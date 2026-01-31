
#include "neocpp/script/script_builder.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/exceptions.hpp"
#include <algorithm>
#include <cstring>
#include <map>

namespace neocpp {

ScriptBuilder& ScriptBuilder::emit(OpCode opcode) {
    script_.push_back(OpCodeHelper::toByte(opcode));
    return *this;
} // namespace neocpp
ScriptBuilder& ScriptBuilder::emitRaw(const Bytes& bytes) {
    script_.insert(script_.end(), bytes.begin(), bytes.end());
    return *this;
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushInteger(int64_t value) {
    if (value == -1) {
        return emit(OpCode::PUSHM1);
    }
    if (value >= 0 && value <= 16) {
        return emit(static_cast<OpCode>(static_cast<uint8_t>(OpCode::PUSH0) + value));
    }

    if (value >= -128 && value <= 127) {
        emit(OpCode::PUSHINT8);
        script_.push_back(static_cast<uint8_t>(value));
    } else if (value >= -32768 && value <= 32767) {
        emit(OpCode::PUSHINT16);
        script_.push_back(value & 0xFF);
        script_.push_back((value >> 8) & 0xFF);
    } else if (value >= -2147483648LL && value <= 2147483647LL) {
        emit(OpCode::PUSHINT32);
        for (int i = 0; i < 4; ++i) {
            script_.push_back((value >> (i * 8)) & 0xFF);
        }
    } else {
        emit(OpCode::PUSHINT64);
        for (int i = 0; i < 8; ++i) {
            script_.push_back((value >> (i * 8)) & 0xFF);
        }
    }

    return *this;
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushData(const Bytes& data) {
    emitPushData(data);
    return *this;
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushString(const std::string& str) {
    Bytes bytes(str.begin(), str.end());
    return pushData(bytes);
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushBool(bool value) {
    return emit(value ? OpCode::PUSH1 : OpCode::PUSH0);
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushNull() {
    return emit(OpCode::PUSHNULL);
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushPublicKey(const SharedPtr<ECPublicKey>& publicKey) {
    return pushData(publicKey->getEncoded());
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushContractParameter(const ContractParameter& parameter) {
    switch (parameter.getType()) {
        case ContractParameterType::BOOLEAN:
            return pushBool(parameter.getBoolean());
        case ContractParameterType::INTEGER:
            return pushInteger(parameter.getInteger());
        case ContractParameterType::BYTE_ARRAY:
            return pushData(parameter.getByteArray());
        case ContractParameterType::STRING:
            return pushString(parameter.getString());
        case ContractParameterType::HASH160:
            return pushData(parameter.getHash160().toLittleEndianArray());
        case ContractParameterType::HASH256:
            return pushData(parameter.getHash256().toLittleEndianArray());
        case ContractParameterType::PUBLIC_KEY:
            return pushPublicKey(parameter.getPublicKey());
        case ContractParameterType::SIGNATURE:
            return pushData(parameter.getByteArray());
        case ContractParameterType::ARRAY:
            return pushArray(parameter.getArray());
        case ContractParameterType::MAP:
            return pushMap(parameter.getMap());
        case ContractParameterType::VOID:
            return pushNull();
        default:
            throw IllegalArgumentException("Unsupported contract parameter type");
    }
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushParam(const ContractParameter& parameter) {
    return pushContractParameter(parameter);
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushArray(const std::vector<ContractParameter>& array) {
    if (array.empty()) {
        return emit(OpCode::NEWARRAY0);
    }

    for (const auto& item : array) {
        pushContractParameter(item);
    }
    return pushInteger(array.size()).emit(OpCode::PACK);
} // namespace neocpp
ScriptBuilder& ScriptBuilder::pushMap(const std::map<ContractParameter, ContractParameter>& paramMap) {
    for (const auto& [key, value] : paramMap) {
        pushContractParameter(key);
        pushContractParameter(value);
    }
    return pushInteger(paramMap.size()).emit(OpCode::PACKMAP);
} // namespace neocpp
ScriptBuilder& ScriptBuilder::callContract(const Hash160& scriptHash, const std::string& method, const std::vector<ContractParameter>& parameters) {
    // Push parameters in reverse order
    for (auto it = parameters.rbegin(); it != parameters.rend(); ++it) {
        pushContractParameter(*it);
    }

    // Push method name
    pushString(method);

    // Push script hash
    pushData(scriptHash.toLittleEndianArray());

    // Emit SYSCALL with System.Contract.Call
    return emitSysCall("System.Contract.Call");
} // namespace neocpp
ScriptBuilder& ScriptBuilder::emitSysCall(const std::string& interopService) {
    emit(OpCode::SYSCALL);
    uint32_t hash = getInteropServiceHash(interopService);
    for (int i = 0; i < 4; ++i) {
        script_.push_back((hash >> (i * 8)) & 0xFF);
    }
    return *this;
} // namespace neocpp
ScriptBuilder& ScriptBuilder::emitJump(OpCode opcode, int offset) {
    emit(opcode);
    if (opcode == OpCode::JMP || opcode == OpCode::JMPIF || opcode == OpCode::JMPIFNOT ||
        opcode == OpCode::JMPEQ || opcode == OpCode::JMPNE || opcode == OpCode::JMPGT ||
        opcode == OpCode::JMPGE || opcode == OpCode::JMPLT || opcode == OpCode::JMPLE ||
        opcode == OpCode::CALL) {
        script_.push_back(static_cast<uint8_t>(offset));
    } else {
        // Long jump
        for (int i = 0; i < 4; ++i) {
            script_.push_back((offset >> (i * 8)) & 0xFF);
        }
    }
    return *this;
} // namespace neocpp
ScriptBuilder& ScriptBuilder::clear() {
    script_.clear();
    return *this;
} // namespace neocpp
Bytes ScriptBuilder::toArray() const {
    return script_;
} // namespace neocpp
Bytes ScriptBuilder::buildVerificationScript(const Bytes& encodedPublicKey) {
    ScriptBuilder builder;
    builder.pushData(encodedPublicKey);
    builder.emitSysCall("System.Crypto.CheckSig");
    return builder.toArray();
} // namespace neocpp
Bytes ScriptBuilder::buildVerificationScript(const SharedPtr<ECPublicKey>& publicKey) {
    return buildVerificationScript(publicKey->getEncoded());
} // namespace neocpp
Bytes ScriptBuilder::buildVerificationScript(const std::vector<SharedPtr<ECPublicKey>>& publicKeys,
                                            int signingThreshold) {
    if (signingThreshold <= 0 || signingThreshold > publicKeys.size()) {
        throw IllegalArgumentException("Invalid signing threshold");
    }

    if (publicKeys.size() > NeoConstants::MAX_PUBLIC_KEYS_PER_MULTISIG_ACCOUNT) {
        throw IllegalArgumentException("Too many public keys");
    }

    ScriptBuilder builder;

    // Push signing threshold
    builder.pushInteger(signingThreshold);

    // Sort public keys
    std::vector<SharedPtr<ECPublicKey>> sortedKeys = publicKeys;
    std::sort(sortedKeys.begin(), sortedKeys.end(),
              [](const SharedPtr<ECPublicKey>& a, const SharedPtr<ECPublicKey>& b) {
                  return a->getEncoded() < b->getEncoded();
              });

    // Push public keys
    for (const auto& key : sortedKeys) {
        builder.pushPublicKey(key);
    }

    // Push number of keys
    builder.pushInteger(sortedKeys.size());

    // Emit CheckMultiSig
    builder.emitSysCall("System.Crypto.CheckMultiSig");

    return builder.toArray();
} // namespace neocpp
Bytes ScriptBuilder::buildMultiSigVerificationScript(const std::vector<Bytes>& publicKeys,
                                                      int signingThreshold) {
    if (signingThreshold <= 0 || signingThreshold > publicKeys.size()) {
        throw IllegalArgumentException("Invalid signing threshold");
    }

    if (publicKeys.size() > NeoConstants::MAX_PUBLIC_KEYS_PER_MULTISIG_ACCOUNT) {
        throw IllegalArgumentException("Too many public keys");
    }

    ScriptBuilder builder;

    // Push signing threshold
    builder.pushInteger(signingThreshold);

    // Sort public keys (by raw bytes)
    std::vector<Bytes> sortedKeys = publicKeys;
    std::sort(sortedKeys.begin(), sortedKeys.end());

    // Push public keys as raw data
    for (const auto& key : sortedKeys) {
        builder.pushData(key);
    }

    // Push number of keys
    builder.pushInteger(sortedKeys.size());

    // Emit CheckMultiSig
    builder.emitSysCall("System.Crypto.CheckMultiSig");

    return builder.toArray();
} // namespace neocpp
Bytes ScriptBuilder::buildInvocationScript(const std::vector<Bytes>& signatures) {
    ScriptBuilder builder;
    for (const auto& signature : signatures) {
        builder.pushData(signature);
    }
    return builder.toArray();
} // namespace neocpp
void ScriptBuilder::emitVarInt(uint64_t value) {
    if (value < 0xFD) {
        script_.push_back(static_cast<uint8_t>(value));
    } else if (value <= 0xFFFF) {
        script_.push_back(0xFD);
        script_.push_back(value & 0xFF);
        script_.push_back((value >> 8) & 0xFF);
    } else if (value <= 0xFFFFFFFF) {
        script_.push_back(0xFE);
        for (int i = 0; i < 4; ++i) {
            script_.push_back((value >> (i * 8)) & 0xFF);
        }
    } else {
        script_.push_back(0xFF);
        for (int i = 0; i < 8; ++i) {
            script_.push_back((value >> (i * 8)) & 0xFF);
        }
    }
} // namespace neocpp
void ScriptBuilder::emitPushData(const Bytes& data) {
    size_t size = data.size();

    // Neo VM push data rules (Neo N3 v3.9.1):
    // 0-75 bytes: raw length byte (0x00-0x4B)
    // 76-255 bytes: PUSHDATA1 (0x12) + 1-byte length
    // 256-65535 bytes: PUSHDATA2 (0x13) + 2-byte length
    // 65536+: PUSHDATA4 (0x14) + 4-byte length
    if (size <= 75) {
        script_.push_back(static_cast<uint8_t>(size));
    } else if (size <= 0xFF) {
        emit(OpCode::PUSHDATA1);
        script_.push_back(static_cast<uint8_t>(size));
    } else if (size <= 0xFFFF) {
        emit(OpCode::PUSHDATA2);
        script_.push_back(size & 0xFF);
        script_.push_back((size >> 8) & 0xFF);
    } else {
        emit(OpCode::PUSHDATA4);
        for (int i = 0; i < 4; ++i) {
            script_.push_back((size >> (i * 8)) & 0xFF);
        }
    }

    script_.insert(script_.end(), data.begin(), data.end());
} // namespace neocpp
uint32_t ScriptBuilder::getInteropServiceHash(const std::string& method) {
    // Calculate SHA256 hash of the method name and take first 4 bytes
    Bytes methodBytes(method.begin(), method.end());
    Bytes hash = HashUtils::sha256(methodBytes);

    uint32_t result = 0;
    for (int i = 0; i < 4; ++i) {
        result |= static_cast<uint32_t>(hash[i]) << (i * 8);
    }

    return result;
} // namespace neocpp
} // namespace neocpp
