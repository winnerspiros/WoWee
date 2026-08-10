/**
 * Android audio stubs — no-crash implementations for all audio interfaces.
 * AudioEngine uses Oboe for real mixing; high-level managers are no-ops.
 */
#include "audio/audio_engine.hpp"
#include "audio/audio_coordinator.hpp"
#include "audio/ui_sound_manager.hpp"
#include "audio/spell_sound_manager.hpp"
#include "audio/music_manager.hpp"
#include "audio/ambient_sound_manager.hpp"
#include "audio/combat_sound_manager.hpp"
#include "audio/movement_sound_manager.hpp"
#include "audio/footstep_manager.hpp"
#include "pipeline/asset_manager.hpp"
#include <string>

namespace wowee::audio {

// --- AudioEngine (singleton, uses Oboe on Android) ---
AudioEngine& AudioEngine::instance() {
    static AudioEngine engine;
    return engine;
}

void AudioEngine::setMasterVolume(float) {}
void AudioEngine::shutdown() {}

// --- UiSoundManager ---
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

// --- SpellSoundManager ---
void SpellSoundManager::playCast(MagicSchool) {}
void SpellSoundManager::playImpact(MagicSchool, SpellPower) {}
void SpellSoundManager::setVolumeScale(float) {}

// --- MusicManager ---
void MusicManager::stopMusic(float) {}
void MusicManager::stopMusic(int) {}
void MusicManager::setVolume(int) {}
void MusicManager::initialize(pipeline::AssetManager*) {}
void MusicManager::update(float) {}
void MusicManager::playFilePath(const std::string&, bool, float) {}
void MusicManager::playZoneMusic(const std::string&, float) {}
void MusicManager::setMusicEnabled(bool) {}

// --- AmbientSoundManager ---
void AmbientSoundManager::setVolumeScale(float) {}

// --- CombatSoundManager ---
void CombatSoundManager::setVolumeScale(float) {}

// --- MovementSoundManager ---
void MovementSoundManager::setVolumeScale(float) {}

// --- AudioCoordinator ---
void AudioCoordinator::onOriginalSoundtrackDisabled(game::ZoneManager*) {}
void AudioCoordinator::setMasterVolume(float) {}
void AudioCoordinator::setMusicVolume(float) {}
void AudioCoordinator::setEffectsVolume(float) {}
void AudioCoordinator::update(float) {}
void AudioCoordinator::shutdown() {}

// --- FootstepManager ---
void FootstepManager::update(float) {}
void FootstepManager::initialize(pipeline::AssetManager*) {}

} // namespace wowee::audio