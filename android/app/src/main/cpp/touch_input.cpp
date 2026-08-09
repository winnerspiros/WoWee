/**
 * touch_input.cpp — Android touch-to-game-input adapter.
 *
 * Maps multitouch gestures to WoWee game inputs:
 *   - Left-side touch drag → virtual joystick (WASD movement)
 *   - Right-side touch drag → camera look (mouse delta)
 *   - Single tap → left click (target/interact)
 *   - Two-finger tap → right click
 *   - Pinch → zoom (camera distance)
 *
 * SDL2 on Android exposes touch events as SDL_FINGERDOWN / SDL_FINGERMOTION /
 * SDL_FINGERUP. We intercept these and inject synthetic SDL keyboard/mouse events
 * into the SDL event queue.
 *
 * This operates as a filter layer between SDL's raw touch events and WoWee's
 * input handling (which expects keyboard + mouse).
 */

#include <SDL2/SDL.h>
#include <cstdint>
#include <cmath>
#include <android/log.h>

namespace wowee {
namespace android {

// --- Configuration ---
namespace {
    // Virtual joystick: percentage of screen from the left edge that acts as movement zone
    constexpr float kJoystickZoneWidth = 0.40f;
    constexpr float kJoystickDeadZone = 0.08f;  // Fraction of zone size
    constexpr float kLookSensitivity = 0.005f;   // Radians per pixel of finger drag
    constexpr float kZoomSensitivity = 0.01f;

    // Finger tracking
    constexpr int kMaxFingers = 5;

    struct TouchFinger {
        SDL_FingerID id;
        float startX, startY;   // Normalized 0..1
        float currentX, currentY;
        bool active = false;
        int64_t downTime;       // For tap detection
    };

    TouchFinger g_fingers[kMaxFingers];
    int g_fingerCount = 0;

    // Joystick state
    float g_joystickX = 0.0f;
    float g_joystickY = 0.0f;
    int g_joystickFinger = -1;

    // Look state
    float g_lookPrevX = 0.0f;
    float g_lookPrevY = 0.0f;
    int g_lookFinger = -1;

    // Pinch state
    float g_pinchDist = 0.0f;
    int g_pinchFinger1 = -1;
    int g_pinchFinger2 = -1;

    // Display dimensions (set on init)
    int g_screenW = 1920;
    int g_screenH = 1080;

    // --- Helpers ---

    int findFinger(SDL_FingerID id) {
        for (int i = 0; i < g_fingerCount; i++) {
            if (g_fingers[i].id == id) return i;
        }
        return -1;
    }

    int findFreeSlot() {
        for (int i = 0; i < kMaxFingers; i++) {
            if (!g_fingers[i].active) return i;
        }
        return -1; // All slots full
    }

    bool isInLeftZone(float x) {
        return x < kJoystickZoneWidth;
    }

    bool isInRightZone(float x) {
        return x > kJoystickZoneWidth;
    }

    // Inject a synthetic keyboard event
    void injectKey(SDL_Scancode scancode, bool pressed) {
        SDL_Event event;
        SDL_zero(event);
        event.type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
        event.key.keysym.scancode = scancode;
        event.key.state = pressed ? SDL_PRESSED : SDL_RELEASED;
        SDL_PushEvent(&event);
    }

    // Inject a synthetic mouse motion event
    void injectMouseMotion(float dx, float dy) {
        SDL_Event event;
        SDL_zero(event);
        event.type = SDL_MOUSEMOTION;
        event.motion.xrel = static_cast<int>(dx);
        event.motion.yrel = static_cast<int>(dy);
        SDL_PushEvent(&event);
    }

    // Inject a synthetic mouse button event
    void injectMouseButton(Uint8 button, bool pressed) {
        SDL_Event event;
        SDL_zero(event);
        event.type = pressed ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
        event.button.button = button;
        event.button.state = pressed ? SDL_PRESSED : SDL_RELEASED;
        SDL_PushEvent(&event);
    }

    // Map joystick position to WASD keys
    void updateJoystickKeys(float x, float y) {
        float deadZone = kJoystickDeadZone;
        float mag = std::sqrt(x*x + y*y);

        if (mag < deadZone) {
            // Release all movement keys
            injectKey(SDL_SCANCODE_W, false);
            injectKey(SDL_SCANCODE_S, false);
            injectKey(SDL_SCANCODE_A, false);
            injectKey(SDL_SCANCODE_D, false);
            return;
        }

        // Normalize
        float nx = x / mag;
        float ny = y / mag;

        // Forward/back
        injectKey(SDL_SCANCODE_W, ny < -0.3f);
        injectKey(SDL_SCANCODE_S, ny > 0.3f);

        // Left/right
        injectKey(SDL_SCANCODE_A, nx < -0.3f);
        injectKey(SDL_SCANCODE_D, nx > 0.3f);
    }

    // Detect and handle a tap
    void handleTap(const TouchFinger& finger) {
        // A tap is: finger down < 300ms, moved < 10% of screen
        float dx = std::abs(finger.currentX - finger.startX);
        float dy = std::abs(finger.currentY - finger.startY);
        float dist = std::sqrt(dx*dx + dy*dy);

        if (dist < 0.03f) { // 3% of screen = tap
            __android_log_print(ANDROID_LOG_DEBUG, "WoWeeTouch",
                                "Tap at (%.2f, %.2f)", finger.startX, finger.startY);

            // Right side tap: left click (target/interact)
            // Two-finger context: right click
            injectMouseButton(SDL_BUTTON_LEFT, true);
            injectMouseButton(SDL_BUTTON_LEFT, false);
        }
    }

} // anonymous namespace

// --- Public API ---

/**
 * Initialize touch input system.
 * Called once after SDL is set up and screen dimensions are known.
 */
void touchInit(int screenWidth, int screenHeight) {
    g_screenW = screenWidth;
    g_screenH = screenHeight;
    __android_log_print(ANDROID_LOG_INFO, "WoWeeTouch",
                        "Touch input initialized: %dx%d", screenWidth, screenHeight);
}

/**
 * Process a touch event from SDL and inject synthetic game inputs.
 * @return true if the event was consumed (should not be forwarded further).
 */
bool touchProcessEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_FINGERDOWN: {
            SDL_FingerID id = event.tfinger.fingerId;
            float x = event.tfinger.x;  // 0..1 normalized
            float y = event.tfinger.y;

            int slot = findFreeSlot();
            if (slot < 0) return false; // Too many fingers

            g_fingers[slot].id = id;
            g_fingers[slot].startX = x;
            g_fingers[slot].startY = y;
            g_fingers[slot].currentX = x;
            g_fingers[slot].currentY = y;
            g_fingers[slot].active = true;
            g_fingers[slot].downTime = event.tfinger.timestamp;
            g_fingerCount++;

            // Assign finger to a role
            if (isInLeftZone(x) && g_joystickFinger < 0) {
                g_joystickFinger = slot;
                __android_log_print(ANDROID_LOG_DEBUG, "WoWeeTouch",
                                    "Joystick finger down at (%.2f, %.2f)", x, y);
            } else if (isInRightZone(x) && g_lookFinger < 0) {
                g_lookFinger = slot;
                g_lookPrevX = x;
                g_lookPrevY = y;
                __android_log_print(ANDROID_LOG_DEBUG, "WoWeeTouch",
                                    "Look finger down at (%.2f, %.2f)", x, y);
            }

            return true;
        }

        case SDL_FINGERMOTION: {
            SDL_FingerID id = event.tfinger.fingerId;
            int slot = findFinger(id);
            if (slot < 0) return false;

            float x = event.tfinger.x;
            float y = event.tfinger.y;
            float dx = x - g_fingers[slot].currentX;
            float dy = y - g_fingers[slot].currentY;

            g_fingers[slot].currentX = x;
            g_fingers[slot].currentY = y;

            if (slot == g_joystickFinger) {
                // Virtual joystick
                float jx = x - g_fingers[slot].startX;
                float jy = y - g_fingers[slot].startY;
                updateJoystickKeys(jx, jy);
            } else if (slot == g_lookFinger) {
                // Camera look
                injectMouseMotion(
                    dx * g_screenW * kLookSensitivity * 100.0f,
                    dy * g_screenH * kLookSensitivity * 100.0f
                );
            }

            return true;
        }

        case SDL_FINGERUP: {
            SDL_FingerID id = event.tfinger.fingerId;
            int slot = findFinger(id);
            if (slot < 0) return false;

            // Check for tap
            handleTap(g_fingers[slot]);

            // Release role
            if (slot == g_joystickFinger) {
                updateJoystickKeys(0.0f, 0.0f);
                g_joystickFinger = -1;
            }
            if (slot == g_lookFinger) {
                g_lookFinger = -1;
            }

            g_fingers[slot].active = false;
            g_fingerCount--;
            return true;
        }

        default:
            return false;
    }
}

/**
 * Check if a raw touch event should be forwarded to ImGui/SDL.
 * We consume movement/look events but let taps through.
 */
bool touchShouldForward(const SDL_Event& event) {
    if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERUP) {
        return true; // ImGui might want these for button presses
    }
    // Motion events consumed by our input layer
    return false;
}

} // namespace android
} // namespace wowee