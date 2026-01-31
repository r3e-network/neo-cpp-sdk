#pragma once

#include <string>
#include <cstdint>

namespace neocpp {

/// Neo VM interop service definitions
class InteropService {
public:
    // System namespace
    static const std::string SYSTEM_BINARY_ATOI;
    static const std::string SYSTEM_BINARY_BASE58_DECODE;
    static const std::string SYSTEM_BINARY_BASE58_ENCODE;
    static const std::string SYSTEM_BINARY_BASE64_DECODE;
    static const std::string SYSTEM_BINARY_BASE64_ENCODE;
    static const std::string SYSTEM_BINARY_DESERIALIZE;
    static const std::string SYSTEM_BINARY_ITOA;
    static const std::string SYSTEM_BINARY_SERIALIZE;

    static const std::string SYSTEM_CONTRACT_CALL;
    static const std::string SYSTEM_CONTRACT_CALL_NATIVE;
    static const std::string SYSTEM_CONTRACT_CREATE;
    static const std::string SYSTEM_CONTRACT_CREATE_MULTISIG_ACCOUNT;
    static const std::string SYSTEM_CONTRACT_CREATE_STANDARD_ACCOUNT;
    static const std::string SYSTEM_CONTRACT_DESTROY;
    static const std::string SYSTEM_CONTRACT_GET_CALL_FLAGS;
    static const std::string SYSTEM_CONTRACT_NativeOnPersist;
    static const std::string SYSTEM_CONTRACT_NativePostPersist;
    static const std::string SYSTEM_CONTRACT_UPDATE;

    static const std::string SYSTEM_CRYPTO_CHECK_MULTISIG;
    static const std::string SYSTEM_CRYPTO_CHECK_SIG;
    static const std::string SYSTEM_CRYPTO_SHA256;
    static const std::string SYSTEM_CRYPTO_RIPEMD160;

    static const std::string SYSTEM_ITERATOR_NEXT;
    static const std::string SYSTEM_ITERATOR_VALUE;

    static const std::string SYSTEM_RUNTIME_BURN_GAS;
    static const std::string SYSTEM_RUNTIME_CHECK_WITNESS;
    static const std::string SYSTEM_RUNTIME_GAS_LEFT;
    static const std::string SYSTEM_RUNTIME_GET_CALLING_SCRIPT_HASH;
    static const std::string SYSTEM_RUNTIME_GET_ENTRY_SCRIPT_HASH;
    static const std::string SYSTEM_RUNTIME_GET_EXECUTING_SCRIPT_HASH;
    static const std::string SYSTEM_RUNTIME_GET_INVOCATION_COUNTER;
    static const std::string SYSTEM_RUNTIME_GET_NETWORK;
    static const std::string SYSTEM_RUNTIME_GET_NOTIFICATIONS;
    static const std::string SYSTEM_RUNTIME_GET_RANDOM;
    static const std::string SYSTEM_RUNTIME_GET_TIME;
    static const std::string SYSTEM_RUNTIME_GET_TRIGGER;
    static const std::string SYSTEM_RUNTIME_LOG;
    static const std::string SYSTEM_RUNTIME_NOTIFY;
    static const std::string SYSTEM_RUNTIME_PLATFORM;

    static const std::string SYSTEM_STORAGE_AS_READONLY;
    static const std::string SYSTEM_STORAGE_DELETE;
    static const std::string SYSTEM_STORAGE_FIND;
    static const std::string SYSTEM_STORAGE_GET;
    static const std::string SYSTEM_STORAGE_GET_CONTEXT;
    static const std::string SYSTEM_STORAGE_GET_READONLY_CONTEXT;
    static const std::string SYSTEM_STORAGE_PUT;

    /// Get the hash of an interop service
    /// @param service The service name
    /// @return The service hash
    static uint32_t getHash(const std::string& service);
};

} // namespace neocpp
