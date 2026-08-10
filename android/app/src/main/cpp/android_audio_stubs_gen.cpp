/**
 * Android audio stubs — exact signatures from headers, no-ops.
 * Only includes symbols referenced in non-audio source files.
 */
#include "audio/activity_sound_manager.hpp"
#include "audio/audio_coordinator.hpp"
#include "audio/music_manager.hpp"
#include "audio/ui_sound_manager.hpp"
#include "audio/spell_sound_manager.hpp"
#include "audio/npc_voice_manager.hpp"
#include "audio/footstep_manager.hpp"
#include "audio/ambient_sound_manager.hpp"
#include "audio/combat_sound_manager.hpp"
#include "audio/movement_sound_manager.hpp"
#include "game/zone_manager.hpp"
#include "pipeline/asset_manager.hpp"
#include <glm/glm.hpp>

namespace wowee::audio {

// --- ActivitySoundManager ---
bool ActivitySoundManager::initialize(pipeline::AssetManager*) { return true; }
void ActivitySoundManager::shutdown() {}
void ActivitySoundManager::update(float) {}
void ActivitySoundManager::playJump() {}
void ActivitySoundManager::playLanding(FootstepSurface, bool) {}
void ActivitySoundManager::setSwimmingState(bool, bool) {}
void ActivitySoundManager::setCharacterVoiceProfile(const std::string&) {}
void ActivitySoundManager::setCharacterVoiceProfile(const std::string&, const std::string&, bool) {}
void ActivitySoundManager::playWaterEnter() {}
void ActivitySoundManager::playWaterExit() {}
void ActivitySoundManager::playMeleeSwing() {}
void ActivitySoundManager::playAttackGrunt() {}
void ActivitySoundManager::playWound(bool) {}
void ActivitySoundManager::playDeath() {}

// --- MusicManager ---
bool MusicManager::initialize(pipeline::AssetManager*) { return true; }
void MusicManager::shutdown() {}
void MusicManager::stopMusic(float) {}
void MusicManager::crossfadeTo(const std::string&, float) {}
void MusicManager::crossfadeToFile(const std::string&, float) {}
void MusicManager::update(float) {}
void MusicManager::setVolume(int) {}
void MusicManager::setUnderwaterMode(bool) {}
void MusicManager::preloadMusic(const std::string&) {}

// --- AudioCoordinator ---
bool AudioCoordinator::initialize() { return true; }
void AudioCoordinator::initializeWithAssets(pipeline::AssetManager*) {}
void AudioCoordinator::shutdown() {}
void AudioCoordinator::updateZoneAudio(const ZoneAudioContext&) {}
void AudioCoordinator::playZoneMusic(const std::string&) {}
void AudioCoordinator::onOriginalSoundtrackDisabled(game::ZoneManager*) {}

// --- UiSoundManager ---
bool UiSoundManager::initialize(pipeline::AssetManager*) { return true; }
void UiSoundManager::shutdown() {}
void UiSoundManager::setVolumeScale(float) {}
void UiSoundManager::playBagOpen() {}
void UiSoundManager::playBagClose() {}
void UiSoundManager::playQuestLogOpen() {}
void UiSoundManager::playQuestLogClose() {}
void UiSoundManager::playCharacterSheetOpen() {}
void UiSoundManager::playCharacterSheetClose() {}
void UiSoundManager::playButtonClick() {}
void UiSoundManager::playMenuButtonClick() {}
void UiSoundManager::playQuestActivate() {}
void UiSoundManager::playQuestComplete() {}
void UiSoundManager::playError() {}

// --- SpellSoundManager ---
bool SpellSoundManager::initialize(pipeline::AssetManager*) { return true; }
void SpellSoundManager::shutdown() {}
void SpellSoundManager::setVolumeScale(float) {}
void SpellSoundManager::playFireball() {}
void SpellSoundManager::playFrostbolt() {}
void SpellSoundManager::playLightningBolt() {}
void SpellSoundManager::playHeal() {}
void SpellSoundManager::playShadowBolt() {}

// --- NpcVoiceManager ---
bool NpcVoiceManager::initialize(pipeline::AssetManager*) { return true; }
void NpcVoiceManager::shutdown() {}
void NpcVoiceManager::playGreeting(uint64_t, VoiceType, const glm::vec3&) {}
void NpcVoiceManager::playFarewell(uint64_t, VoiceType, const glm::vec3&) {}
void NpcVoiceManager::playVendor(uint64_t, VoiceType, const glm::vec3&) {}
void NpcVoiceManager::playPissed(uint64_t, VoiceType, const glm::vec3&) {}
void NpcVoiceManager::playFlee(uint64_t, VoiceType, const glm::vec3&) {}

// --- FootstepManager ---
bool FootstepManager::initialize(pipeline::AssetManager*) { return true; }
void FootstepManager::shutdown() {}
void FootstepManager::update(float) {}
void FootstepManager::playFootstep(FootstepSurface, bool) {}

// --- AmbientSoundManager ---
bool AmbientSoundManager::initialize(pipeline::AssetManager*) { return true; }
void AmbientSoundManager::shutdown() {}
void AmbientSoundManager::update(float, const glm::vec3&, bool, bool, bool) {}
void AmbientSoundManager::setWeather(WeatherType) {}
void AmbientSoundManager::setZoneType(ZoneType) {}
void AmbientSoundManager::setZoneId(uint32_t) {}
void AmbientSoundManager::setCityType(CityType) {}
void AmbientSoundManager::setVolumeScale(float) {}

// --- CombatSoundManager ---
bool CombatSoundManager::initialize(pipeline::AssetManager*) { return true; }
void CombatSoundManager::shutdown() {}
void CombatSoundManager::setVolumeScale(float) {}

// --- MovementSoundManager ---
bool MovementSoundManager::initialize(pipeline::AssetManager*) { return true; }
void MovementSoundManager::shutdown() {}
void MovementSoundManager::setVolumeScale(float) {}

} // namespace wowee::audio