#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>
#include <vector>

namespace neocpp {

/// Role management native contract
class RoleManagement : public SmartContract {
public:
    static const Hash160 SCRIPT_HASH;
    static const std::string NAME;

    /// Role types
    enum class Role : uint8_t {
        StateValidator = 4,
        Oracle = 8,
        NeoFSAlphabetNode = 16,
        P2PNotary = 32
    };

    /// Constructor
    explicit RoleManagement(const SharedPtr<NeoRpcClient>& client);

    /// Create instance
    static SharedPtr<RoleManagement> create(const SharedPtr<NeoRpcClient>& client);

    /// Get designated nodes by role
    /// @param role The role
    /// @param blockIndex The block index
    /// @return List of designated public keys
    std::vector<std::string> getDesignatedByRole(Role role, uint32_t blockIndex);

    /// Designate nodes as role
    /// @param role The role
    /// @param publicKeys The public keys
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> designateAsRole(Role role, const std::vector<std::string>& publicKeys);
};

} // namespace neocpp
