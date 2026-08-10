/** Android link-time stubs for excluded modules (OpenSSL, FFmpeg). */
#include "auth/auth_handler.hpp"
#include "auth/crypto.hpp"
#include "audio/audio_engine.hpp"
#include "audio/audio_coordinator.hpp"
#include "audio/ui_sound_manager.hpp"
#include "audio/spell_sound_manager.hpp"
#include "audio/music_manager.hpp"
#include "audio/ambient_sound_manager.hpp"
#include "audio/combat_sound_manager.hpp"
#include "audio/movement_sound_manager.hpp"
#include "audio/footstep_manager.hpp"
#include <string>

namespace wowee {

// --- auth stubs ---
namespace auth {
    std::vector<uint8_t> Crypto::sha1(const std::vector<uint8_t>&) { return {}; }

    void AuthHandler::requestRealmList() {}
    void AuthHandler::submitSecurityCode(const std::string&) {}
    std::string AuthHandler::getSessionKey() const { return ""; }
    bool AuthHandler::isConnected() const { return false; }
    void AuthHandler::disconnect() {}
    void AuthHandler::update() {}
}  // namespace auth

// --- audio stubs ---
namespace audio {
    AudioEngine& AudioEngine::instance() { static AudioEngine e; return e; }
    void AudioEngine::setMasterVolume(float) {}
    void AudioEngine::shutdown() {}

    void UiSoundManager::playError() {}
    void UiSoundManager::playQuestActivate() {}
    void UiSoundManager::playTargetSelect() {}
    void UiSoundManager::playMailReceived() {}
    void UiSoundManager::playLootItem() {}
    void UiSoundManager::playQuestComplete() {}
    void UiSoundManager::playLevelUp() {}
    void UiSoundManager::playAchievementAlert() {}
    void UiSoundManager::playWhisperReceived() {}
    void UiSoundManager::setVolumeScale(float) {}

    void SpellSoundManager::playCast(MagicSchool) {}
    void SpellSoundManager::playImpact(MagicSchool, SpellPower) {}
    void SpellSoundManager::setVolumeScale(float) {}

    void MusicManager::stopMusic(float) {}
    void MusicManager::setVolume(int) {}
    void MusicManager::initialize(wowee::pipeline::AssetManager*) {}
    void MusicManager::update(float) {}
    void MusicManager::playFilePath(const std::string&, bool, float) {}
    void MusicManager::playZoneMusic(const std::string&, float) {}
    void MusicManager::stopMusic(int) {}
    void MusicManager::setMusicEnabled(bool) {}

    void AmbientSoundManager::setVolumeScale(float) {}
    void CombatSoundManager::setVolumeScale(float) {}
    void MovementSoundManager::setVolumeScale(float) {}

    void AudioCoordinator::onOriginalSoundtrackDisabled(wowee::game::ZoneManager*) {}
    void AudioCoordinator::setMasterVolume(float) {}
    void AudioCoordinator::setMusicVolume(float) {}
    void AudioCoordinator::setEffectsVolume(float) {}
    void AudioCoordinator::update(float) {}
    void AudioCoordinator::shutdown() {}

    void FootstepManager::update(float) {}
    void FootstepManager::initialize(wowee::pipeline::AssetManager*) {}
}  // namespace audio

// --- config stubs ---
namespace core {
    std::string getConfigRoot() { return ""; }
}  // namespace core

// --- SDL2 joystick stubs ---
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