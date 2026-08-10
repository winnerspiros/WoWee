/** Android link-time stubs for excluded modules (OpenSSL, FFmpeg-dependent audio).
 *  Most symbols are left undefined and resolved at runtime (allowed via
 *  --allow-shlib-undefined linker flag). Only stub what must be stubbed for
 *  compilation to pass. */

namespace wowee {

// --- SDL2 joystick stubs (not compiled in SDL2-static) ---
extern "C" {
    void Android_OnPadDown(int, int) {}
    void Android_OnPadUp(int, int) {}
    void Android_OnJoy(int, int, float) {}
    void Android_OnHat(int, int, int) {}
    int Android_AddJoystick(int, const char*, const char*, int, int, int, int, int, int, int) { return -1; }
    int Android_RemoveJoystick(int) { return -1; }
    int Android_AddHaptic(int, const char*, int, int) { return -1; }
    int Android_RemoveHaptic(int) { return -1; }
}

}  // namespace wowee