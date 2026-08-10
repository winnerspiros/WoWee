/** Android link-time stubs for excluded modules. */

#include "game/warden_handler.hpp"
#include "game/warden_crypto.hpp"
#include "game/warden_memory.hpp"
#include "game/warden_module.hpp"
#include "network/packet.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <cstdint>

namespace wowee {

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

} // namespace wowee

// Warden stubs auto-generated from headers below:
namespace wowee::game {

// --- WardenHandler ---
explicit WardenHandler::WardenHandler(GameHandler& owner) { return {}; }
void WardenHandler::registerOpcodes(DispatchTable& table) {}
void WardenHandler::reset() {}
void WardenHandler::initModuleManager() {}
void WardenHandler::update(float deltaTime) {}
void WardenHandler::handleWardenData(network::Packet& packet) {}
bool WardenHandler::loadWardenCRFile(const std::string& moduleHashHex) { return false; }
// --- WardenCrypto ---
bool WardenCrypto::initFromSessionKey(const std::vector<uint8_t>& sessionKey) { return false; }
void WardenCrypto::replaceKeys(const std::vector<uint8_t>& newEncryptKey, const std::vector<uint8_t>& newDecryptKey) {}
std::vector<uint8_t> WardenCrypto::decrypt(const std::vector<uint8_t>& data) { return 0; }
std::vector<uint8_t> WardenCrypto::encrypt(const std::vector<uint8_t>& data) { return 0; }
void WardenCrypto::initRC4(const std::vector<uint8_t>& key, std::vector<uint8_t>& state, uint8_t& i, uint8_t& j) {}
void WardenCrypto::processRC4(const uint8_t* input, uint8_t* output, size_t length, std::vector<uint8_t>& state, uint8_t& i, uint8_t& j) {}
void WardenCrypto::sha1RandxGenerate(const std::vector<uint8_t>& seed, uint8_t* outputEncryptKey, uint8_t* outputDecryptKey) {}
// --- WardenMemory ---
bool WardenMemory::load(uint16_t build, bool isTurtle) { return false; }
bool WardenMemory::loadFromFile(const std::string& exePath) { return false; }
bool WardenMemory::readMemory(uint32_t va, uint8_t length, uint8_t* outBuf) const { return false; }
bool WardenMemory::searchCodePattern(const uint8_t seed[4], const uint8_t expectedHash[20], uint8_t patternLen, bool imageOnly, uint32_t hintOffset, bool hintOnly) const { return false; }
void WardenMemory::writeLE32(uint32_t va, uint32_t value) {}
bool WardenMemory::parsePE(const std::vector<uint8_t>& fileData) { return false; }
void WardenMemory::initKuserSharedData() {}
void WardenMemory::patchRuntimeGlobals() {}
void WardenMemory::patchTurtleWowBinary() {}
void WardenMemory::verifyWardenScanEntries() {}
std::string WardenMemory::findWowExe(uint16_t build) const { return {}; }
uint32_t WardenMemory::expectedImageSizeForBuild(uint16_t build, bool isTurtle) { return 0; }
// --- WardenModule ---
bool WardenModule::load(const std::vector<uint8_t>& moduleData, const std::vector<uint8_t>& md5Hash, const std::vector<uint8_t>& rc4Key) { return false; }
bool WardenModule::processCheckRequest(const std::vector<uint8_t>& checkData, std::vector<uint8_t>& responseOut) { return false; }
uint32_t WardenModule::tick(uint32_t deltaMs) { return 0; }
void WardenModule::generateRC4Keys(uint8_t* packet) {}
void WardenModule::unload() {}
void WardenModule::setCallbackDependencies(WardenCrypto* crypto, SendPacketFunc sendFunc) {}
bool WardenModule::verifyMD5(const std::vector<uint8_t>& data, const std::vector<uint8_t>& expectedHash) { return false; }
bool WardenModule::decryptRC4(const std::vector<uint8_t>& encrypted, const std::vector<uint8_t>& key, std::vector<uint8_t>& decryptedOut) { return false; }
bool WardenModule::verifyRSASignature(const std::vector<uint8_t>& data) { return false; }
bool WardenModule::decompressZlib(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressedOut) { return false; }
bool WardenModule::parseExecutableFormat(const std::vector<uint8_t>& exeData) { return false; }
bool WardenModule::applyRelocations() { return false; }
bool WardenModule::bindAPIs() { return false; }
bool WardenModule::initializeModule() { return false; }
// --- WardenModuleManager ---
bool WardenModuleManager::hasModule(const std::vector<uint8_t>& md5Hash) { return false; }
std::shared_ptr<WardenModule> WardenModuleManager::getModule(const std::vector<uint8_t>& md5Hash) { return nullptr; }
bool WardenModuleManager::receiveModuleChunk(const std::vector<uint8_t>& md5Hash, const std::vector<uint8_t>& chunkData, bool isComplete) { return false; }
bool WardenModuleManager::cacheModule(const std::vector<uint8_t>& md5Hash, const std::vector<uint8_t>& moduleData) { return false; }
bool WardenModuleManager::loadCachedModule(const std::vector<uint8_t>& md5Hash, std::vector<uint8_t>& moduleDataOut) { return false; }
std::string WardenModuleManager::getCachePath(const std::vector<uint8_t>& md5Hash) { return {}; }

} // namespace wowee::game
