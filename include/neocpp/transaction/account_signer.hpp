#pragma once

#include "neocpp/transaction/signer.hpp"
#include <memory>

namespace neocpp {

// Forward declarations
class Account;
class Witness;

/// Account-based transaction signer
class AccountSigner : public Signer {
private:
    SharedPtr<Account> account_;

public:
    /// Constructor
    /// @param account The account to sign with
    /// @param scopes The witness scopes
    AccountSigner(const SharedPtr<Account>& account, WitnessScope scopes = WitnessScope::CALLED_BY_ENTRY);

    /// Destructor
    ~AccountSigner() = default;

    /// Get the account
    const SharedPtr<Account>& getAccount() const { return account_; }

    /// Create witness for this signer
    /// @param message The message to sign
    /// @return The witness
    SharedPtr<Witness> createWitness(const Bytes& message) const;

    /// Sign a message
    /// @param message The message to sign
    /// @return The signature
    Bytes sign(const Bytes& message) const;

    /// Creates a signer for the given account with no witness scope
    /// @param account The signer account
    /// @return The signer
    static SharedPtr<AccountSigner> none(const SharedPtr<Account>& account);

    /// Creates a signer for the given account with calledByEntry scope
    /// @param account The signer account
    /// @return The signer
    static SharedPtr<AccountSigner> calledByEntry(const SharedPtr<Account>& account);

    /// Creates a signer for the given account with global scope
    /// @param account The signer account
    /// @return The signer
    static SharedPtr<AccountSigner> global(const SharedPtr<Account>& account);
};

} // namespace neocpp
