/**
 * AudioEngine stub for Android — no-op singleton to prevent SIGSEGV.
 * Real implementation uses miniaudio (not available on NDK).
 */
#include "audio/audio_engine.hpp"
#include <glm/glm.hpp>

namespace wowee::audio {

// Singleton
AudioEngine& AudioEngine::instance() {
    static AudioEngine engine;
    return engine;
}

AudioEngine::AudioEngine() = default;
AudioEngine::~AudioEngine() = default;

bool AudioEngine::initialize() { return true; }
void AudioEngine::shutdown() {}

void AudioEngine::setMasterVolume(float) {}
void AudioEngine::setListenerPosition(const glm::vec3&) {}
void AudioEngine::setListenerOrientation(const glm::vec3&, const glm::vec3&) {}

bool AudioEngine::playSound2D(const std::vector<uint8_t>&, float, float) { return false; }
bool AudioEngine::playSound2D(const std::string&, float, float) { return false; }
uint32_t AudioEngine::playSound2DStoppable(const std::vector<uint8_t>&, float) { return 0; }
void AudioEngine::stopSound(uint32_t) {}

bool AudioEngine::playSound3D(const std::vector<uint8_t>&, const glm::vec3&, float, float, float) { return false; }
bool AudioEngine::playSound3D(const std::string&, const glm::vec3&, float, float, float) { return false; }

bool AudioEngine::playMusic(std::shared_ptr<const std::vector<uint8_t>>, float, bool) { return false; }
void AudioEngine::stopMusic() {}
bool AudioEngine::isMusicPlaying() const { return false; }
void AudioEngine::setMusicVolume(float) {}

void AudioEngine::update(float) {}

} // namespace wowee::audio