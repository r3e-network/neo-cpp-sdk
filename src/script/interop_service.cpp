
#include "neocpp/script/interop_service.hpp"
#include "neocpp/crypto/hash.hpp"

namespace neocpp {

// System.Binary
const std::string InteropService::SYSTEM_BINARY_ATOI = "System.Binary.Atoi";
const std::string InteropService::SYSTEM_BINARY_BASE58_DECODE = "System.Binary.Base58Decode";
const std::string InteropService::SYSTEM_BINARY_BASE58_ENCODE = "System.Binary.Base58Encode";
const std::string InteropService::SYSTEM_BINARY_BASE64_DECODE = "System.Binary.Base64Decode";
const std::string InteropService::SYSTEM_BINARY_BASE64_ENCODE = "System.Binary.Base64Encode";
const std::string InteropService::SYSTEM_BINARY_DESERIALIZE = "System.Binary.Deserialize";
const std::string InteropService::SYSTEM_BINARY_ITOA = "System.Binary.Itoa";
const std::string InteropService::SYSTEM_BINARY_SERIALIZE = "System.Binary.Serialize";

// System.Contract
const std::string InteropService::SYSTEM_CONTRACT_CALL = "System.Contract.Call";
const std::string InteropService::SYSTEM_CONTRACT_CALL_NATIVE = "System.Contract.CallNative";
const std::string InteropService::SYSTEM_CONTRACT_CREATE = "System.Contract.Create";
const std::string InteropService::SYSTEM_CONTRACT_CREATE_MULTISIG_ACCOUNT = "System.Contract.CreateMultisigAccount";
const std::string InteropService::SYSTEM_CONTRACT_CREATE_STANDARD_ACCOUNT = "System.Contract.CreateStandardAccount";
const std::string InteropService::SYSTEM_CONTRACT_DESTROY = "System.Contract.Destroy";
const std::string InteropService::SYSTEM_CONTRACT_GET_CALL_FLAGS = "System.Contract.GetCallFlags";
const std::string InteropService::SYSTEM_CONTRACT_NativeOnPersist = "System.Contract.NativeOnPersist";
const std::string InteropService::SYSTEM_CONTRACT_NativePostPersist = "System.Contract.NativePostPersist";
const std::string InteropService::SYSTEM_CONTRACT_UPDATE = "System.Contract.Update";

// System.Crypto
const std::string InteropService::SYSTEM_CRYPTO_CHECK_MULTISIG = "System.Crypto.CheckMultisig";
const std::string InteropService::SYSTEM_CRYPTO_CHECK_SIG = "System.Crypto.CheckSig";
const std::string InteropService::SYSTEM_CRYPTO_SHA256 = "System.Crypto.Sha256";
const std::string InteropService::SYSTEM_CRYPTO_RIPEMD160 = "System.Crypto.Ripemd160";

// System.Iterator
const std::string InteropService::SYSTEM_ITERATOR_NEXT = "System.Iterator.Next";
const std::string InteropService::SYSTEM_ITERATOR_VALUE = "System.Iterator.Value";

// System.Runtime
const std::string InteropService::SYSTEM_RUNTIME_BURN_GAS = "System.Runtime.BurnGas";
const std::string InteropService::SYSTEM_RUNTIME_CHECK_WITNESS = "System.Runtime.CheckWitness";
const std::string InteropService::SYSTEM_RUNTIME_GAS_LEFT = "System.Runtime.GasLeft";
const std::string InteropService::SYSTEM_RUNTIME_GET_CALLING_SCRIPT_HASH = "System.Runtime.GetCallingScriptHash";
const std::string InteropService::SYSTEM_RUNTIME_GET_ENTRY_SCRIPT_HASH = "System.Runtime.GetEntryScriptHash";
const std::string InteropService::SYSTEM_RUNTIME_GET_EXECUTING_SCRIPT_HASH = "System.Runtime.GetExecutingScriptHash";
const std::string InteropService::SYSTEM_RUNTIME_GET_INVOCATION_COUNTER = "System.Runtime.GetInvocationCounter";
const std::string InteropService::SYSTEM_RUNTIME_GET_NETWORK = "System.Runtime.GetNetwork";
const std::string InteropService::SYSTEM_RUNTIME_GET_NOTIFICATIONS = "System.Runtime.GetNotifications";
const std::string InteropService::SYSTEM_RUNTIME_GET_RANDOM = "System.Runtime.GetRandom";
const std::string InteropService::SYSTEM_RUNTIME_GET_TIME = "System.Runtime.GetTime";
const std::string InteropService::SYSTEM_RUNTIME_GET_TRIGGER = "System.Runtime.GetTrigger";
const std::string InteropService::SYSTEM_RUNTIME_LOG = "System.Runtime.Log";
const std::string InteropService::SYSTEM_RUNTIME_NOTIFY = "System.Runtime.Notify";
const std::string InteropService::SYSTEM_RUNTIME_PLATFORM = "System.Runtime.Platform";

// System.Storage
const std::string InteropService::SYSTEM_STORAGE_AS_READONLY = "System.Storage.AsReadOnly";
const std::string InteropService::SYSTEM_STORAGE_DELETE = "System.Storage.Delete";
const std::string InteropService::SYSTEM_STORAGE_FIND = "System.Storage.Find";
const std::string InteropService::SYSTEM_STORAGE_GET = "System.Storage.Get";
const std::string InteropService::SYSTEM_STORAGE_GET_CONTEXT = "System.Storage.GetContext";
const std::string InteropService::SYSTEM_STORAGE_GET_READONLY_CONTEXT = "System.Storage.GetReadOnlyContext";
const std::string InteropService::SYSTEM_STORAGE_PUT = "System.Storage.Put";

uint32_t InteropService::getHash(const std::string& service) {
    // Compute SHA256 of the service name and take first 4 bytes as little-endian
    Bytes methodBytes(service.begin(), service.end());
    Bytes hash = HashUtils::sha256(methodBytes);
    
    uint32_t result = 0;
    for (int i = 0; i < 4; ++i) {
        result |= static_cast<uint32_t>(hash[i]) << (i * 8);
    }
    return result;
}

} // namespace neocpp
