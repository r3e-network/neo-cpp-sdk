#pragma once

#include "neocpp/transaction/signer.hpp"

namespace neocpp {

/// Contract-based transaction signer
class ContractSigner : public Signer {
private:
    Bytes verificationScript_;
    
public:
    /// Constructor
    /// @param contractHash The contract script hash
    /// @param scopes The witness scopes
    ContractSigner(const Hash160& contractHash, WitnessScope scopes = WitnessScope::CALLED_BY_ENTRY);
    
    /// Constructor with verification script
    /// @param contractHash The contract script hash
    /// @param verificationScript The verification script
    /// @param scopes The witness scopes
    ContractSigner(const Hash160& contractHash, const Bytes& verificationScript, 
                   WitnessScope scopes = WitnessScope::CALLED_BY_ENTRY);
    
    /// Destructor
    ~ContractSigner() = default;
    
    /// Get the verification script
    const Bytes& getVerificationScript() const { return verificationScript_; }
    
    /// Set the verification script
    void setVerificationScript(const Bytes& script) { verificationScript_ = script; }
    
    /// Creates a signer for the given contract with calledByEntry scope
    /// @param contractHash The contract script hash
    /// @return The signer
    static SharedPtr<ContractSigner> calledByEntry(const Hash160& contractHash);
};

} // namespace neocpp