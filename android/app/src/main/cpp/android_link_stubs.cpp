/** Android link-time stubs for excluded modules. */
#include "auth/crypto.hpp"
#include "audio/ui_sound_manager.hpp"
#include "audio/spell_sound_manager.hpp"

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
}  // namespace audio

}  // namespace wowee