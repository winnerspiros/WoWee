/**
 * Android audio stubs — no-crash no-ops with matching header signatures.
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
#include "game/zone_manager.hpp"
#include "pipeline/asset_manager.hpp"
#include <string>

namespace wowee::audio {

// --- AudioEngine ---
AudioEngine& AudioEngine::instance() {
    static AudioEngine e;
    return e;
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
bool MusicManager::initialize(pipeline::AssetManager*) { return true; }
void MusicManager::stopMusic(float) {}
void MusicManager::setVolume(int) {}
void MusicManager::update(float) {}
void MusicManager::playFilePath(const std::string&, bool, float) {}

// --- Ambient/Combat/Movement ---
void AmbientSoundManager::setVolumeScale(float) {}
void CombatSoundManager::setVolumeScale(float) {}
void MovementSoundManager::setVolumeScale(float) {}

// --- AudioCoordinator ---
bool AudioCoordinator::initialize() { return true; }
void AudioCoordinator::initializeWithAssets(pipeline::AssetManager*) {}
void AudioCoordinator::updateZoneAudio(const ZoneAudioContext&) {}
void AudioCoordinator::playZoneMusic(const std::string&) {}
void AudioCoordinator::onOriginalSoundtrackDisabled(game::ZoneManager*) {}
void AudioCoordinator::shutdown() {}

// --- FootstepManager ---
bool FootstepManager::initialize(pipeline::AssetManager*) { return true; }
void FootstepManager::update(float) {}

} // namespace wowee::audio