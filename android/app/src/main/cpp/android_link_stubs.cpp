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

namespace wowee::game {

// --- WardenHandler ---
void WardenHandler::registerOpcodes(DispatchTable&) {}
void WardenHandler::reset() {}
void WardenHandler::initModuleManager() {}
void WardenHandler::update(float) {}
void WardenHandler::handleWardenData(network::Packet&) {}
bool WardenHandler::loadWardenCRFile(const std::string&) { return false; }

// --- WardenCrypto ---
bool WardenCrypto::initFromSessionKey(const std::vector<uint8_t>&) { return false; }
void WardenCrypto::replaceKeys(const std::vector<uint8_t>&, const std::vector<uint8_t>&) {}
std::vector<uint8_t> WardenCrypto::decrypt(const std::vector<uint8_t>&) { return {}; }
std::vector<uint8_t> WardenCrypto::encrypt(const std::vector<uint8_t>&) { return {}; }
void WardenCrypto::initRC4(const std::vector<uint8_t>&, std::vector<uint8_t>&, uint8_t&, uint8_t&) {}
void WardenCrypto::processRC4(const uint8_t*, uint8_t*, size_t, std::vector<uint8_t>&, uint8_t&, uint8_t&) {}
void WardenCrypto::sha1RandxGenerate(const std::vector<uint8_t>&, uint8_t*, uint8_t*) {}

// --- WardenMemory ---
bool WardenMemory::load(uint16_t, bool) { return false; }
bool WardenMemory::loadFromFile(const std::string&) { return false; }
bool WardenMemory::readMemory(uint32_t, uint8_t, uint8_t*) const { return false; }
bool WardenMemory::searchCodePattern(const uint8_t*, const uint8_t*, uint8_t, bool, uint32_t, bool) const { return false; }
void WardenMemory::writeLE32(uint32_t, uint32_t) {}
bool WardenMemory::parsePE(const std::vector<uint8_t>&) { return false; }
void WardenMemory::initKuserSharedData() {}
void WardenMemory::patchRuntimeGlobals() {}
void WardenMemory::patchTurtleWowBinary() {}
void WardenMemory::verifyWardenScanEntries() {}
std::string WardenMemory::findWowExe(uint16_t) const { return {}; }
uint32_t WardenMemory::expectedImageSizeForBuild(uint16_t, bool) { return 0; }

// --- WardenModule ---
bool WardenModule::generateRC4Keys(uint8_t*) { return false; }
void WardenModule::unload() {}
void WardenModule::setCallbackDependencies(WardenCrypto*, std::function<void(const std::vector<uint8_t>&)>) {}
bool WardenModule::verifyMD5(const std::vector<uint8_t>&, const std::vector<uint8_t>&) { return false; }

// --- WardenModuleManager ---
bool WardenModuleManager::hasModule(uint32_t) const { return false; }
std::shared_ptr<WardenModule> WardenModuleManager::getModule(uint32_t) { return nullptr; }
bool WardenModuleManager::receiveModuleChunk(uint32_t, uint32_t, uint32_t, const std::vector<uint8_t>&) { return false; }
bool WardenModuleManager::cacheModule(uint32_t, const std::vector<uint8_t>&) { return false; }
bool WardenModuleManager::loadCachedModule(uint32_t) { return false; }
std::string WardenModuleManager::getCachePath(uint32_t) { return {}; }

} // namespace wowee::game
