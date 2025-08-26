#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include "neocpp/types/types.hpp"
#include "neocpp/script/op_code.hpp"

namespace neocpp {

// Forward declarations
class Hash160;
class ECPublicKey;
class ContractParameter;

/// Builder class for creating Neo VM scripts
class ScriptBuilder {
private:
    std::vector<uint8_t> script_;
    
public:
    ScriptBuilder() = default;
    ~ScriptBuilder() = default;
    
    /// Emit an opcode
    /// @param opcode The opcode to emit
    /// @return Reference to this builder
    ScriptBuilder& emit(OpCode opcode);
    
    /// Emit raw bytes
    /// @param bytes The bytes to emit
    /// @return Reference to this builder
    ScriptBuilder& emitRaw(const Bytes& bytes);
    
    /// Push integer onto the stack
    /// @param value The integer value
    /// @return Reference to this builder
    ScriptBuilder& pushInteger(int64_t value);
    
    /// Push bytes onto the stack
    /// @param data The data to push
    /// @return Reference to this builder
    ScriptBuilder& pushData(const Bytes& data);
    
    /// Push string onto the stack
    /// @param str The string to push
    /// @return Reference to this builder
    ScriptBuilder& pushString(const std::string& str);
    
    /// Push boolean onto the stack
    /// @param value The boolean value
    /// @return Reference to this builder
    ScriptBuilder& pushBool(bool value);
    
    /// Push null onto the stack
    /// @return Reference to this builder
    ScriptBuilder& pushNull();
    
    /// Push public key onto the stack
    /// @param publicKey The public key
    /// @return Reference to this builder
    ScriptBuilder& pushPublicKey(const SharedPtr<ECPublicKey>& publicKey);
    
    /// Push contract parameter onto the stack
    /// @param parameter The contract parameter
    /// @return Reference to this builder
    ScriptBuilder& pushContractParameter(const ContractParameter& parameter);
    
    /// Push contract parameter onto the stack (alias for pushContractParameter)
    /// @param parameter The contract parameter
    /// @return Reference to this builder
    ScriptBuilder& pushParam(const ContractParameter& parameter);
    
    /// Push array of contract parameters onto the stack
    /// @param array The array of contract parameters
    /// @return Reference to this builder
    ScriptBuilder& pushArray(const std::vector<ContractParameter>& array);
    
    /// Push map of contract parameters onto the stack
    /// @param paramMap The map of contract parameters
    /// @return Reference to this builder
    ScriptBuilder& pushMap(const std::map<ContractParameter, ContractParameter>& paramMap);
    
    /// Call a contract
    /// @param scriptHash The contract script hash
    /// @param method The method name
    /// @param parameters The parameters
    /// @return Reference to this builder
    ScriptBuilder& callContract(const Hash160& scriptHash, const std::string& method, const std::vector<ContractParameter>& parameters);
    
    /// Emit a SYSCALL
    /// @param interopService The interop service name
    /// @return Reference to this builder
    ScriptBuilder& emitSysCall(const std::string& interopService);
    
    /// Emit a JMP instruction
    /// @param offset The jump offset
    /// @return Reference to this builder
    ScriptBuilder& emitJump(OpCode opcode, int offset);
    
    /// Clear the script
    /// @return Reference to this builder
    ScriptBuilder& clear();
    
    /// Get the built script
    /// @return The script bytes
    Bytes toArray() const;
    
    /// Get the current script size
    /// @return The size in bytes
    size_t size() const { return script_.size(); }
    
    /// Build a verification script for a single public key
    /// @param publicKey The public key
    /// @return The verification script
    static Bytes buildVerificationScript(const Bytes& encodedPublicKey);
    static Bytes buildVerificationScript(const SharedPtr<ECPublicKey>& publicKey);
    
    /// Build a multi-signature verification script
    /// @param publicKeys The public keys
    /// @param signingThreshold The minimum number of signatures required
    /// @return The verification script
    static Bytes buildVerificationScript(const std::vector<SharedPtr<ECPublicKey>>& publicKeys, int signingThreshold);
    
    /// Build an invocation script for signatures
    /// @param signatures The signatures
    /// @return The invocation script
    static Bytes buildInvocationScript(const std::vector<Bytes>& signatures);
    
private:
    /// Emit variable length integer
    void emitVarInt(uint64_t value);
    
    /// Emit push data with appropriate opcode
    void emitPushData(const Bytes& data);
    
    /// Get the hash of an interop service
    uint32_t getInteropServiceHash(const std::string& method);
};

} // namespace neocpp