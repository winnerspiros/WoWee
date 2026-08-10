/** Android link-time stubs for excluded modules. */
#include "auth/crypto.hpp"
#include "audio/ui_sound_manager.hpp"
#include "audio/spell_sound_manager.hpp"
#include "audio/music_manager.hpp"

namespace wowee {

// --- auth::Crypto stubs (OpenSSL excluded) ---
namespace auth {
    std::vector<uint8_t> Crypto::sha1(const std::vector<uint8_t>&) {
        return {};
    }
}  // namespace auth

// --- audio stubs (no FFmpeg on Android) ---
namespace audio {
    void UiSoundManager::playError() {}
    void UiSoundManager::playQuestActivate() {}
    void UiSoundManager::playTargetSelect() {}
    void UiSoundManager::playMailReceived() {}
    void UiSoundManager::playLootItem() {}
    void UiSoundManager::playQuestComplete() {}

    void SpellSoundManager::playCast(MagicSchool) {}
    void SpellSoundManager::playImpact(MagicSchool, SpellPower) {}
    void MusicManager::stopMusic(float) {}
    void UiSoundManager::playLevelUp() {}
}  // namespace audio

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