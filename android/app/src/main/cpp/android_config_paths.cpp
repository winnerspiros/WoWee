/**
 * android_config_paths.cpp — Android-specific config & data path resolution.
 *
 * On Android, paths differ from desktop:
 *   - Config: app-private internal storage (context.getFilesDir())
 *   - Data:   WOW_DATA_PATH env var set by WoWeeActivity
 *   - Executable: not relevant (loaded as .so)
 *
 * This file overrides the default config_paths.cpp implementations
 * when WOWEE_ANDROID is defined.
 */

#include "core/config_paths.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>

#ifdef WOWEE_ANDROID

namespace wowee::core {

namespace fs = std::filesystem;

std::string getExecutableDir() {
    // On Android, there's no traditional executable directory.
    // Return the data path as a sensible fallback.
    const char* dataPath = std::getenv("WOW_DATA_PATH");
    if (dataPath && *dataPath) {
        return std::string(dataPath);
    }
    return "/data/data/com.wowee.app/files";
}

std::string getConfigDir() {
    // Use app-private files directory for config
    const char* dataPath = std::getenv("WOW_DATA_PATH");
    if (dataPath && *dataPath) {
        // Config goes alongside data in a .wowee subdirectory
        return std::string(dataPath) + "/.wowee";
    }
    return "/data/data/com.wowee.app/files/.wowee";
}

std::string getDataDir() {
    const char* dataPath = std::getenv("WOW_DATA_PATH");
    if (dataPath && *dataPath) {
        return std::string(dataPath);
    }
    return "/data/data/com.wowee.app/files/Data";
}

std::string getCacheDir() {
    return getConfigDir() + "/cache";
}

std::string getLogDir() {
    return getConfigDir() + "/logs";
}

std::string getConfigRoot() {
    return getConfigDir();
}

} // namespace wowee::core

#endif // WOWEE_ANDROID