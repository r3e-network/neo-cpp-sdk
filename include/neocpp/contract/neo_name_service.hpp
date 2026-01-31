#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>
#include <chrono>

namespace neocpp {

/// Neo Name Service (NNS) contract interface
class NeoNameService : public SmartContract {
public:
    /// NNS contract script hash on MainNet
    static const Hash160 SCRIPT_HASH;

    /// Constructor
    /// @param client The RPC client
    explicit NeoNameService(const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    ~NeoNameService() = default;

    /// Get the owner of a domain
    /// @param domain The domain name
    /// @return The owner address
    std::string getOwner(const std::string& domain);

    /// Resolve a domain to an address
    /// @param domain The domain name
    /// @param type The record type (default: 1 for address)
    /// @return The resolved value
    std::string resolve(const std::string& domain, uint16_t type = 1);

    /// Get all properties of a domain
    /// @param domain The domain name
    /// @return The domain properties
    nlohmann::json getProperties(const std::string& domain);

    /// Check if a domain is available
    /// @param domain The domain name
    /// @return True if available
    bool isAvailable(const std::string& domain);

    /// Register a new domain
    /// @param account The account to register with
    /// @param domain The domain name
    /// @param years The registration period in years
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> registerName(const SharedPtr<Account>& account,
                                          const std::string& domain,
                                          int years = 1);

    /// Renew a domain
    /// @param account The owner account
    /// @param domain The domain name
    /// @param years Additional years to add
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> renew(const SharedPtr<Account>& account,
                                       const std::string& domain,
                                       int years = 1);

    /// Set domain owner
    /// @param account The current owner account
    /// @param domain The domain name
    /// @param newOwner The new owner address
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> setOwner(const SharedPtr<Account>& account,
                                          const std::string& domain,
                                          const std::string& newOwner);

    /// Set domain record
    /// @param account The owner account
    /// @param domain The domain name
    /// @param type The record type
    /// @param value The record value
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> setRecord(const SharedPtr<Account>& account,
                                           const std::string& domain,
                                           uint16_t type,
                                           const std::string& value);

    /// Get domain price
    /// @param length The domain length
    /// @return The price in GAS
    [[nodiscard]] int64_t getPrice(int length);

    /// Get domain expiration time
    /// @param domain The domain name
    /// @return The expiration timestamp
    std::chrono::system_clock::time_point getExpiration(const std::string& domain);
};

} // namespace neocpp
