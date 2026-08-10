/**
 * android_main.cpp — WoWee Android entry point (SDL_main).
 *
 * SDL2 on Android: SDLActivity loads this shared library and calls
 * SDL_main(int argc, char* argv[]) on a dedicated thread after the
 * Surface is initialized. No JNI bridge needed.
 */
#include "core/application.hpp"
#include "core/config_paths.hpp"
#include "core/logger.hpp"
#include "core/version.hpp"

#include <SDL2/SDL.h>
#include <android/log.h>
#include <cstdlib>
#include <exception>

// Touch input adapter
namespace wowee::android {
    void touchInit(int screenWidth, int screenHeight);
    bool touchProcessEvent(const SDL_Event& event);
}

namespace {

void androidLogCallback(void*, int /*category*/, SDL_LogPriority priority,
                        const char* message) {
    int prio;
    switch (priority) {
        case SDL_LOG_PRIORITY_VERBOSE: prio = ANDROID_LOG_VERBOSE; break;
        case SDL_LOG_PRIORITY_DEBUG:   prio = ANDROID_LOG_DEBUG;   break;
        case SDL_LOG_PRIORITY_INFO:    prio = ANDROID_LOG_INFO;    break;
        case SDL_LOG_PRIORITY_WARN:    prio = ANDROID_LOG_WARN;    break;
        case SDL_LOG_PRIORITY_ERROR:   prio = ANDROID_LOG_ERROR;   break;
        case SDL_LOG_PRIORITY_CRITICAL:prio = ANDROID_LOG_FATAL;   break;
        default:                        prio = ANDROID_LOG_DEFAULT; break;
    }
    __android_log_print(prio, "WoWee", "%s", message);
}

} // namespace

// --- SDL_main: called by SDLActivity after JNI Surface is ready ---

extern "C" int SDL_main(int /*argc*/, char* /*argv*/[]) {
    __android_log_print(ANDROID_LOG_INFO, "WoWee", "SDL_main starting");

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    SDL_LogSetOutputFunction(androidLogCallback, nullptr);

    SDL_Log("WoWee %s — Android arm64-v8a", WOWEE_VERSION_STRING);
    SDL_Log("Data path: %s", getenv("WOW_DATA_PATH") ?: "(not set)");

    try {
        wowee::core::Application app;

        if (!app.initialize()) {
            SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                          "Application::initialize() failed");
            return 1;
        }

        // Touch input filter
        auto* win = app.getWindow();
        if (win && win->getSDLWindow()) {
            wowee::android::touchInit(win->getWidth(), win->getHeight());
            SDL_SetEventFilter([](void*, SDL_Event* event) -> int {
                return wowee::android::touchProcessEvent(*event) ? 0 : 1;
            }, nullptr);
            SDL_Log("Touch filter registered (%dx%d)",
                    win->getWidth(), win->getHeight());
        }

        SDL_Log("Entering main loop...");
        app.run();
        SDL_Log("Main loop ended. Shutting down...");
        app.shutdown();

    } catch (const std::exception& e) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                      "Exception: %s", e.what());
        return 1;
    } catch (...) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                      "Unknown exception");
        return 1;
    }

    SDL_Log("SDL_main exiting normally");
    return 0;
}