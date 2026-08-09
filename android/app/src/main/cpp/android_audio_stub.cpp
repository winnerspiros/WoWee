/**
 * android_audio_stub.cpp — Stub audio implementation for Android.
 *
 * The desktop WoWee uses FFmpeg for audio decoding and SDL2 audio for output.
 * On Android, we initially stub out all audio. A future implementation could use:
 *   - AAudio (Android's low-latency audio API, API 26+)
 *   - OpenSL ES (legacy, API 9+)
 *   - Oboe (Google's C++ wrapper for AAudio/OpenSL ES)
 *   - MediaCodec for decoding MP3/OGG assets
 *
 * For the initial port, audio is disabled. The game is still fully playable
 * without audio — all networking, rendering, input, and UI work independently.
 *
 * The stub satisfies link-time dependencies for all the audio source files.
 * When we enable audio, we replace this file with real AAudio/Oboe implementations
 * and link against the FFmpeg Android binaries.
 */

#include "audio/audio_engine.hpp"
#include "audio/audio_coordinator.hpp"
#include "audio/music_manager.hpp"
#include "audio/footstep_manager.hpp"
#include "audio/activity_sound_manager.hpp"
#include "audio/mount_sound_manager.hpp"
#include "audio/npc_voice_manager.hpp"
#include "audio/player_voice_manager.hpp"
#include "audio/ambient_sound_manager.hpp"
#include "audio/ui_sound_manager.hpp"
#include "audio/combat_sound_manager.hpp"
#include "audio/spell_sound_manager.hpp"
#include "audio/movement_sound_manager.hpp"
#include "core/logger.hpp"

namespace wowee {
namespace audio {

// --- AudioEngine stub ---

AudioEngine::AudioEngine() {
    LOG_INFO("Android: AudioEngine stubbed (no FFmpeg/AAudio yet)");
}

AudioEngine::~AudioEngine() = default;

bool AudioEngine::initialize() {
    LOG_INFO("Android: AudioEngine::initialize() — no-op");
    return true;
}

void AudioEngine::shutdown() {
    LOG_INFO("Android: AudioEngine::shutdown() — no-op");
}

void AudioEngine::playSound(const std::string& /*path*/, float /*volume*/) {}
void AudioEngine::stopAll() {}
void AudioEngine::setMasterVolume(float) {}
void AudioEngine::update() {}

// --- AudioCoordinator stub ---

AudioCoordinator::AudioCoordinator() = default;
AudioCoordinator::~AudioCoordinator() = default;

bool AudioCoordinator::initialize() { return true; }
void AudioCoordinator::shutdown() {}
void AudioCoordinator::update() {}

// --- MusicManager stub ---

MusicManager::MusicManager() = default;
MusicManager::~MusicManager() = default;

bool MusicManager::initialize() { return true; }
void MusicManager::shutdown() {}
void MusicManager::update() {}
void MusicManager::playZoneMusic(uint32_t /*zoneId*/) {}
void MusicManager::stop() {}

// --- FootstepManager stub ---

FootstepManager::FootstepManager() = default;
FootstepManager::~FootstepManager() = default;

bool FootstepManager::initialize() { return true; }
void FootstepManager::shutdown() {}
void FootstepManager::update() {}
void FootstepManager::onPlayerMove(const std::string& /*terrainType*/) {}

// --- ActivitySoundManager stub ---

ActivitySoundManager::ActivitySoundManager() = default;
ActivitySoundManager::~ActivitySoundManager() = default;

bool ActivitySoundManager::initialize() { return true; }
void ActivitySoundManager::shutdown() {}
void ActivitySoundManager::update() {}

// --- MountSoundManager stub ---

MountSoundManager::MountSoundManager() = default;
MountSoundManager::~MountSoundManager() = default;

bool MountSoundManager::initialize() { return true; }
void MountSoundManager::shutdown() {}
void MountSoundManager::update() {}

// --- NPCVoiceManager stub ---

NPCVoiceManager::NPCVoiceManager() = default;
NPCVoiceManager::~NPCVoiceManager() = default;

bool NPCVoiceManager::initialize() { return true; }
void NPCVoiceManager::shutdown() {}
void NPCVoiceManager::update() {}

// --- PlayerVoiceManager stub ---

PlayerVoiceManager::PlayerVoiceManager() = default;
PlayerVoiceManager::~PlayerVoiceManager() = default;

bool PlayerVoiceManager::initialize() { return true; }
void PlayerVoiceManager::shutdown() {}
void PlayerVoiceManager::update() {}

// --- AmbientSoundManager stub ---

AmbientSoundManager::AmbientSoundManager() = default;
AmbientSoundManager::~AmbientSoundManager() = default;

bool AmbientSoundManager::initialize() { return true; }
void AmbientSoundManager::shutdown() {}
void AmbientSoundManager::update() {}

// --- UISoundManager stub ---

UISoundManager::UISoundManager() = default;
UISoundManager::~UISoundManager() = default;

bool UISoundManager::initialize() { return true; }
void UISoundManager::shutdown() {}
void UISoundManager::update() {}
void UISoundManager::playUISound(const std::string& /*soundId*/) {}

// --- CombatSoundManager stub ---

CombatSoundManager::CombatSoundManager() = default;
CombatSoundManager::~CombatSoundManager() = default;

bool CombatSoundManager::initialize() { return true; }
void CombatSoundManager::shutdown() {}
void CombatSoundManager::update() {}

// --- SpellSoundManager stub ---

SpellSoundManager::SpellSoundManager() = default;
SpellSoundManager::~SpellSoundManager() = default;

bool SpellSoundManager::initialize() { return true; }
void SpellSoundManager::shutdown() {}
void SpellSoundManager::update() {}

// --- MovementSoundManager stub ---

MovementSoundManager::MovementSoundManager() = default;
MovementSoundManager::~MovementSoundManager() = default;

bool MovementSoundManager::initialize() { return true; }
void MovementSoundManager::shutdown() {}
void MovementSoundManager::update() {}

} // namespace audio
} // namespace wowee