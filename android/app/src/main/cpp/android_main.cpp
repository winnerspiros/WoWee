/**
 * android_main.cpp — WoWee Android entry point.
 *
 * Called from WoWeeActivity.java via JNI (nativeInit / nativeShutdown).
 *
 * Architecture:
 *   Java WoWeeActivity → JNI nativeInit() → Application::initialize() → Application::run()
 *
 * SDL2 on Android: the Java SDLActivity/SDL.setSurface manages the native window.
 * Application::initialize() internally creates its Window (SDL_CreateWindow),
 * which on Android maps to the existing SurfaceView from SDL.setSurface().
 *
 * Application::run() contains the main game loop. It blocks until shutdown.
 */

#include "core/application.hpp"
#include "core/config_paths.hpp"
#include "core/logger.hpp"

#include <SDL2/SDL.h>
#include <android/log.h>
#include <jni.h>
#include <string>
#include <thread>
#include <atomic>
#include <exception>

// --- Android logging bridge ---
namespace {
    void androidLogCallback(void*, int /*category*/, SDL_LogPriority priority,
                            const char* message) {
        int androidPriority;
        switch (priority) {
            case SDL_LOG_PRIORITY_VERBOSE: androidPriority = ANDROID_LOG_VERBOSE; break;
            case SDL_LOG_PRIORITY_DEBUG:   androidPriority = ANDROID_LOG_DEBUG;   break;
            case SDL_LOG_PRIORITY_INFO:    androidPriority = ANDROID_LOG_INFO;    break;
            case SDL_LOG_PRIORITY_WARN:    androidPriority = ANDROID_LOG_WARN;    break;
            case SDL_LOG_PRIORITY_ERROR:   androidPriority = ANDROID_LOG_ERROR;   break;
            case SDL_LOG_PRIORITY_CRITICAL:androidPriority = ANDROID_LOG_FATAL;   break;
            default:                        androidPriority = ANDROID_LOG_DEFAULT; break;
        }
        __android_log_print(androidPriority, "WoWeeNative", "%s", message);
    }
}

// --- Global state ---
static std::atomic<bool> g_running{false};
static std::thread g_mainThread;
static std::string g_dataPath;

// --- Native functions: called from WoWeeActivity.java via JNI ---

extern "C" {

JNIEXPORT void JNICALL
Java_com_wowee_app_WoWeeActivity_nativeInit(
    JNIEnv* env, jclass /* cls */,
    jstring dataPath, jstring externalPath) {

    const char* dataPathStr = env->GetStringUTFChars(dataPath, nullptr);
    const char* externalPathStr = env->GetStringUTFChars(externalPath, nullptr);

    g_dataPath = dataPathStr;

    // Set environment variables for WoWee
    setenv("WOW_DATA_PATH", dataPathStr, 1);
    setenv("WOWEE_EXTERNAL_PATH", externalPathStr, 1);
    setenv("WOWEE_ANDROID", "1", 1);
    setenv("HOME", externalPathStr, 1); // config paths use HOME on Linux

    env->ReleaseStringUTFChars(dataPath, dataPathStr);
    env->ReleaseStringUTFChars(externalPath, externalPathStr);

    __android_log_print(ANDROID_LOG_INFO, "WoWeeNative",
                        "nativeInit: data=%s", g_dataPath.c_str());

    // Redirect SDL logging to logcat
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    SDL_LogSetOutputFunction(androidLogCallback, nullptr);

    g_running.store(true);

    // Run WoWee on a dedicated thread. Application::run() blocks
    // the calling thread with the main game loop.
    g_mainThread = std::thread([]() {
        __android_log_print(ANDROID_LOG_INFO, "WoWeeNative",
                            "Main thread starting WoWee...");

        try {
            wowee::core::Application app;

            if (!app.initialize()) {
                __android_log_print(ANDROID_LOG_FATAL, "WoWeeNative",
                                    "Application::initialize() failed");
                g_running.store(false);
                return;
            }

            __android_log_print(ANDROID_LOG_INFO, "WoWeeNative",
                                "Application initialized. Entering main loop...");

            // Blocks until the application quits
            app.run();

            __android_log_print(ANDROID_LOG_INFO, "WoWeeNative",
                                "Main loop ended. Shutting down...");

            app.shutdown();

        } catch (const std::exception& e) {
            __android_log_print(ANDROID_LOG_FATAL, "WoWeeNative",
                                "Exception: %s", e.what());
        } catch (...) {
            __android_log_print(ANDROID_LOG_FATAL, "WoWeeNative",
                                "Unknown exception");
        }

        g_running.store(false);
        __android_log_print(ANDROID_LOG_INFO, "WoWeeNative",
                            "Native thread exiting");
    });
}

JNIEXPORT void JNICALL
Java_com_wowee_app_WoWeeActivity_nativeShutdown(
    JNIEnv* /* env */, jclass /* cls */) {

    __android_log_print(ANDROID_LOG_INFO, "WoWeeNative", "nativeShutdown");

    g_running.store(false);

    // SDL_PushEvent with SDL_QUIT to break out of Application::run()
    SDL_Event quitEvent;
    SDL_zero(quitEvent);
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);

    if (g_mainThread.joinable()) {
        g_mainThread.join();
    }

    __android_log_print(ANDROID_LOG_INFO, "WoWeeNative", "Shutdown complete");
}

} // extern "C"