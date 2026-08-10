/**
 * AUTO-GENERATED Android audio stubs.
 * Generated from include/audio/*.hpp — 13 audio classes, all methods no-op.
 * AudioEngine singleton defined separately.
 */

#include "audio/activity_sound_manager.hpp"
#include "audio/ambient_sound_manager.hpp"
#include "audio/audio_coordinator.hpp"
#include "audio/audio_engine.hpp"
#include "audio/combat_sound_manager.hpp"
#include "audio/footstep_manager.hpp"
#include "audio/mount_sound_manager.hpp"
#include "audio/movement_sound_manager.hpp"
#include "audio/music_manager.hpp"
#include "audio/npc_voice_manager.hpp"
#include "audio/player_voice_manager.hpp"
#include "audio/spell_sound_manager.hpp"
#include "audio/ui_sound_manager.hpp"
#include "game/zone_manager.hpp"
#include "pipeline/asset_manager.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace wowee {
namespace audio {

// --- ActivitySoundManager ---
ActivitySoundManager::ActivitySoundManager() {}
ActivitySoundManager::~ActivitySoundManager() {}
bool ActivitySoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void ActivitySoundManager::shutdown() {}
void ActivitySoundManager::update(float deltaTime) {}
void ActivitySoundManager::playJump() {}
void ActivitySoundManager::playLanding(FootstepSurface surface, bool hardLanding) {}
void ActivitySoundManager::setSwimmingState(bool swimming, bool moving) {}
void ActivitySoundManager::setCharacterVoiceProfile(const std::string& modelName) {}
void ActivitySoundManager::setCharacterVoiceProfile(const std::string& raceFolder, const std::string& raceBase, bool male) {}
void ActivitySoundManager::playWaterEnter() {}
void ActivitySoundManager::playWaterExit() {}
void ActivitySoundManager::playMeleeSwing() {}
void ActivitySoundManager::playAttackGrunt() {}
void ActivitySoundManager::playWound(bool isCrit = false) {}
void ActivitySoundManager::playDeath() {}
void ActivitySoundManager::preloadCandidates(std::vector<Sample>& out, const std::vector<std::string>& candidates) {}
void ActivitySoundManager::preloadLandingSet(FootstepSurface surface, const std::string& material) {}
void ActivitySoundManager::rebuildJumpClipsForProfile(const std::string& raceFolder, const std::string& raceBase, bool male) {}
void ActivitySoundManager::rebuildSwimLoopClipsForProfile(const std::string& raceFolder, const std::string& raceBase, bool male) {}
void ActivitySoundManager::rebuildHardLandClipsForProfile(const std::string& raceFolder, const std::string& raceBase, bool male) {}
void ActivitySoundManager::rebuildCombatVocalClipsForProfile(const std::string& raceFolder, const std::string& raceBase, bool male) {}
bool ActivitySoundManager::playSplash(const std::vector<Sample>& clips) { return false; }
void ActivitySoundManager::startSwimLoop() {}
void ActivitySoundManager::stopSwimLoop() {}
void ActivitySoundManager::stopOneShot() {}
void ActivitySoundManager::reapProcesses() {}

// --- AmbientSoundManager ---
AmbientSoundManager::AmbientSoundManager() {}
AmbientSoundManager::~AmbientSoundManager() {}
bool AmbientSoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void AmbientSoundManager::shutdown() {}
void AmbientSoundManager::update(float deltaTime, const glm::vec3& cameraPos, bool isIndoor, bool isSwimming = false, bool isBlacksmith = false) {}
void AmbientSoundManager::setWeather(WeatherType type) {}
void AmbientSoundManager::setZoneType(ZoneType type) {}
void AmbientSoundManager::setZoneId(uint32_t zoneId) {}
void AmbientSoundManager::setCityType(CityType type) {}
uint64_t AmbientSoundManager::addEmitter(const glm::vec3& position, AmbientType type) { return 0; }
void AmbientSoundManager::removeEmitter(uint64_t id) {}
void AmbientSoundManager::clearEmitters() {}
void AmbientSoundManager::setGameTime(float hours) {}
void AmbientSoundManager::setVolumeScale(float scale) {}
void AmbientSoundManager::updatePositionalEmitters(float deltaTime, const glm::vec3& cameraPos) {}
void AmbientSoundManager::updatePeriodicSounds(float deltaTime, bool isIndoor, bool isSwimming) {}
void AmbientSoundManager::updateWindAmbience(float deltaTime, bool isIndoor) {}
void AmbientSoundManager::updateBlacksmithAmbience(float deltaTime) {}
void AmbientSoundManager::updateWeatherAmbience(float deltaTime, bool isIndoor) {}
void AmbientSoundManager::updateWaterAmbience(float deltaTime, bool isSwimming) {}
void AmbientSoundManager::updateZoneAmbience(float deltaTime, bool isIndoor) {}
void AmbientSoundManager::updateCityAmbience(float deltaTime) {}
void AmbientSoundManager::updateBellTolls(float deltaTime) {}
bool AmbientSoundManager::loadSound(const std::string& path, AmbientSample& sample, pipeline::AssetManager* assets) { return false; }

// --- AudioCoordinator ---
AudioCoordinator::AudioCoordinator() {}
AudioCoordinator::~AudioCoordinator() {}
bool AudioCoordinator::initialize() { return false; }
void AudioCoordinator::initializeWithAssets(pipeline::AssetManager* assetManager) {}
void AudioCoordinator::shutdown() {}
void AudioCoordinator::updateZoneAudio(const ZoneAudioContext& ctx) {}
void AudioCoordinator::onOriginalSoundtrackDisabled(game::ZoneManager* zm) {}
void AudioCoordinator::playZoneMusic(const std::string& music) {}

// --- AudioEngine ---
AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() {}
bool AudioEngine::initialize() { return false; }
void AudioEngine::shutdown() {}
void AudioEngine::setMasterVolume(float volume) {}
void AudioEngine::setListenerPosition(const glm::vec3& position) {}
void AudioEngine::setListenerOrientation(const glm::vec3& forward, const glm::vec3& up) {}
bool AudioEngine::playSound2D(const std::vector<uint8_t>& wavData, float volume = 1.0f, float pitch = 1.0f) { return false; }
bool AudioEngine::playSound2D(const std::string& mpqPath, float volume = 1.0f, float pitch = 1.0f) { return false; }
uint32_t AudioEngine::playSound2DStoppable(const std::vector<uint8_t>& wavData, float volume = 1.0f) { return 0; }
void AudioEngine::stopSound(uint32_t id) {}
bool AudioEngine::playSound3D(const std::vector<uint8_t>& wavData, const glm::vec3& position,
                     float volume = 1.0f, float pitch = 1.0f, float maxDistance = 100.0f) { return false; }
bool AudioEngine::playSound3D(const std::string& mpqPath, const glm::vec3& position,
                     float volume = 1.0f, float pitch = 1.0f, float maxDistance = 100.0f) { return false; }
bool AudioEngine::playMusic(std::shared_ptr<const std::vector<uint8_t>> musicData,
                   float volume = 1.0f, bool loop = true) { return false; }
void AudioEngine::stopMusic() {}
bool AudioEngine::isMusicPlaying() const { return false; }
void AudioEngine::setMusicVolume(float volume) {}
void AudioEngine::update(float deltaTime) {}

// --- CombatSoundManager ---
CombatSoundManager::CombatSoundManager() {}
CombatSoundManager::~CombatSoundManager() {}
bool CombatSoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void CombatSoundManager::shutdown() {}
void CombatSoundManager::setVolumeScale(float scale) {}
void CombatSoundManager::playWeaponSwing(WeaponSize size, bool isCrit = false) {}
void CombatSoundManager::playWeaponMiss(bool twoHanded = false) {}
void CombatSoundManager::playImpact(WeaponSize weaponSize, ImpactType impactType, bool isCrit = false) {}
void CombatSoundManager::playClap() {}
void CombatSoundManager::playPlayerAttackGrunt(PlayerRace race) {}
void CombatSoundManager::playPlayerWound(PlayerRace race, bool isCrit = false) {}
void CombatSoundManager::playPlayerDeath(PlayerRace race) {}
bool CombatSoundManager::loadSound(const std::string& path, CombatSample& sample, pipeline::AssetManager* assets) { return false; }
void CombatSoundManager::playSound(const std::vector<CombatSample>& library, float volumeMultiplier = 1.0f) {}
void CombatSoundManager::playRandomSound(const std::vector<CombatSample>& library, float volumeMultiplier = 1.0f) {}

// --- FootstepManager ---
FootstepManager::FootstepManager() {}
FootstepManager::~FootstepManager() {}
bool FootstepManager::initialize(pipeline::AssetManager* assets) { return false; }
void FootstepManager::shutdown() {}
void FootstepManager::update(float deltaTime) {}
void FootstepManager::playFootstep(FootstepSurface surface, bool sprinting) {}
void FootstepManager::playMountFootstep(FootstepSurface surface, FootstepBank bank) {}
void FootstepManager::preloadSurface(SurfaceSamples* bank, FootstepSurface surface,
                        const std::vector<std::string>& candidates, const char* bankName) {}
bool FootstepManager::playRandomStep(FootstepSurface surface, FootstepBank bank,
                        float minInterval, float volumeMul) { return false; }
static const char* FootstepManager::surfaceName(FootstepSurface surface) { return nullptr; }

// --- MountSoundManager ---
MountSoundManager::MountSoundManager() {}
MountSoundManager::~MountSoundManager() {}
bool MountSoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void MountSoundManager::shutdown() {}
void MountSoundManager::update(float deltaTime) {}
void MountSoundManager::onMount(uint32_t creatureDisplayId, bool isFlying, const std::string& modelPath = "") {}
void MountSoundManager::onDismount() {}
void MountSoundManager::setMoving(bool moving) {}
void MountSoundManager::setFlying(bool flying) {}
void MountSoundManager::setGrounded(bool grounded) {}
void MountSoundManager::playRearUpSound() {}
void MountSoundManager::playJumpSound() {}
void MountSoundManager::playLandSound() {}
void MountSoundManager::playIdleSound() {}
MountType MountSoundManager::detectMountType(uint32_t creatureDisplayId) const { return {}; }
MountFamily MountSoundManager::detectMountFamily(uint32_t creatureDisplayId) const { return {}; }
MountFamily MountSoundManager::detectMountFamilyFromPath(const std::string& modelPath) const { return {}; }
void MountSoundManager::updateMountSounds() {}
void MountSoundManager::stopAllMountSounds() {}
void MountSoundManager::loadMountSounds() {}
bool MountSoundManager::loadSound(const std::string& path, MountSample& sample) { return false; }
const FamilySounds& MountSoundManager::getCurrentFamilySounds() const { return {}; }

// --- MovementSoundManager ---
MovementSoundManager::MovementSoundManager() {}
MovementSoundManager::~MovementSoundManager() {}
bool MovementSoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void MovementSoundManager::shutdown() {}
void MovementSoundManager::setVolumeScale(float scale) {}
void MovementSoundManager::playEnterWater(CharacterSize size) {}
void MovementSoundManager::playWaterFootstep(CharacterSize size) {}
void MovementSoundManager::playJump(PlayerRace race) {}
void MovementSoundManager::playLand(PlayerRace race) {}
bool MovementSoundManager::loadSound(const std::string& path, MovementSample& sample, pipeline::AssetManager* assets) { return false; }
void MovementSoundManager::playSound(const std::vector<MovementSample>& library, float volumeMultiplier = 1.0f) {}
void MovementSoundManager::playRandomSound(const std::vector<MovementSample>& library, float volumeMultiplier = 1.0f) {}

// --- MusicManager ---
MusicManager::MusicManager() {}
MusicManager::~MusicManager() {}
bool MusicManager::initialize(pipeline::AssetManager* assets) { return false; }
void MusicManager::shutdown() {}
void MusicManager::stopMusic(float fadeMs = 2000.0f) {}
void MusicManager::crossfadeTo(const std::string& mpqPath, float fadeMs = 3000.0f) {}
void MusicManager::crossfadeToFile(const std::string& filePath, float fadeMs = 3000.0f) {}
void MusicManager::update(float deltaTime) {}
void MusicManager::setVolume(int volume) {}
void MusicManager::setUnderwaterMode(bool underwater) {}
void MusicManager::preloadMusic(const std::string& mpqPath) {}
float MusicManager::effectiveMusicVolume() const { return 0.0f; }
void MusicManager::cancelPendingFileLoad() {}
void MusicManager::pollPendingFileLoad() {}

// --- NpcVoiceManager ---
NpcVoiceManager::NpcVoiceManager() {}
NpcVoiceManager::~NpcVoiceManager() {}
bool NpcVoiceManager::initialize(pipeline::AssetManager* assets) { return false; }
void NpcVoiceManager::shutdown() {}
void NpcVoiceManager::playGreeting(uint64_t npcGuid, VoiceType voiceType, const glm::vec3& position) {}
void NpcVoiceManager::playFarewell(uint64_t npcGuid, VoiceType voiceType, const glm::vec3& position) {}
void NpcVoiceManager::playVendor(uint64_t npcGuid, VoiceType voiceType, const glm::vec3& position) {}
void NpcVoiceManager::playPissed(uint64_t npcGuid, VoiceType voiceType, const glm::vec3& position) {}
void NpcVoiceManager::playAggro(uint64_t npcGuid, uint32_t displayId, VoiceType voiceType,
                   const glm::vec3& position) {}
void NpcVoiceManager::playCombatAttack(uint64_t npcGuid, uint32_t displayId,
                          const glm::vec3& position) {}
void NpcVoiceManager::playFlee(uint64_t npcGuid, VoiceType voiceType, const glm::vec3& position) {}
void NpcVoiceManager::loadVoiceSounds() {}
void NpcVoiceManager::loadCreatureAggroSounds() {}
bool NpcVoiceManager::loadSound(const std::string& path, VoiceSample& sample) { return false; }
bool NpcVoiceManager::playSoundEntry(uint32_t soundId, const glm::vec3& position) { return false; }
void NpcVoiceManager::playSound(uint64_t npcGuid, VoiceType voiceType, SoundCategory category, const glm::vec3& position) {}

// --- PlayerVoiceManager ---
PlayerVoiceManager::PlayerVoiceManager() {}
PlayerVoiceManager::~PlayerVoiceManager() {}
bool PlayerVoiceManager::initialize(pipeline::AssetManager* assets) { return false; }
void PlayerVoiceManager::shutdown() {}
void PlayerVoiceManager::setVolumeScale(float scale) {}
void PlayerVoiceManager::playError(PlayerErrorSpeech type, uint8_t raceId, uint8_t gender) {}
void PlayerVoiceManager::ensureLibrary(uint8_t raceId, uint8_t gender) {}

// --- SpellSoundManager ---
SpellSoundManager::SpellSoundManager() {}
SpellSoundManager::~SpellSoundManager() {}
bool SpellSoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void SpellSoundManager::shutdown() {}
void SpellSoundManager::setVolumeScale(float scale) {}
void SpellSoundManager::playPrecast(MagicSchool school, SpellPower power) {}
void SpellSoundManager::stopPrecast() {}
void SpellSoundManager::playCast(MagicSchool school) {}
void SpellSoundManager::playImpact(MagicSchool school, SpellPower power) {}
void SpellSoundManager::playFireball() {}
void SpellSoundManager::playFrostbolt() {}
void SpellSoundManager::playLightningBolt() {}
void SpellSoundManager::playHeal() {}
void SpellSoundManager::playShadowBolt() {}
bool SpellSoundManager::loadSound(const std::string& path, SpellSample& sample, pipeline::AssetManager* assets) { return false; }
void SpellSoundManager::playSound(const std::vector<SpellSample>& library, float volumeMultiplier = 1.0f) {}
void SpellSoundManager::playRandomSound(const std::vector<SpellSample>& library, float volumeMultiplier = 1.0f) {}

// --- UiSoundManager ---
UiSoundManager::UiSoundManager() {}
UiSoundManager::~UiSoundManager() {}
bool UiSoundManager::initialize(pipeline::AssetManager* assets) { return false; }
void UiSoundManager::shutdown() {}
void UiSoundManager::setVolumeScale(float scale) {}
void UiSoundManager::playBagOpen() {}
void UiSoundManager::playBagClose() {}
void UiSoundManager::playQuestLogOpen() {}
void UiSoundManager::playQuestLogClose() {}
void UiSoundManager::playCharacterSheetOpen() {}
void UiSoundManager::playCharacterSheetClose() {}
void UiSoundManager::playAuctionHouseOpen() {}
void UiSoundManager::playAuctionHouseClose() {}
void UiSoundManager::playGuildBankOpen() {}
void UiSoundManager::playGuildBankClose() {}
void UiSoundManager::playButtonClick() {}
void UiSoundManager::playMenuButtonClick() {}
void UiSoundManager::playQuestActivate() {}
void UiSoundManager::playQuestComplete() {}
void UiSoundManager::playQuestFailed() {}
void UiSoundManager::playQuestUpdate() {}
void UiSoundManager::playFishingBite() {}
void UiSoundManager::playLootCoinSmall() {}
void UiSoundManager::playLootCoinLarge() {}
void UiSoundManager::playLootItem() {}
void UiSoundManager::playDropOnGround() {}
void UiSoundManager::playPickupBag() {}
void UiSoundManager::playPickupBook() {}
void UiSoundManager::playPickupCloth() {}
void UiSoundManager::playPickupFood() {}
void UiSoundManager::playPickupGem() {}
void UiSoundManager::playEating() {}
void UiSoundManager::playDrinking() {}
void UiSoundManager::playLevelUp() {}
void UiSoundManager::playAchievementAlert() {}
void UiSoundManager::playError() {}
void UiSoundManager::playTargetSelect() {}
void UiSoundManager::playTargetDeselect() {}
void UiSoundManager::playWhisperReceived() {}
void UiSoundManager::playMailReceived() {}
void UiSoundManager::playMinimapPing() {}
bool UiSoundManager::loadSound(const std::string& path, UISample& sample, pipeline::AssetManager* assets) { return false; }
void UiSoundManager::playSound(const std::vector<UISample>& library) {}

// --- AudioEngine singleton ---
AudioEngine& AudioEngine::instance() {
    static AudioEngine inst;
    return inst;
}

} // namespace audio
} // namespace wowee