#pragma once

#include <vector>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Invocation script for transaction witnesses
class InvocationScript {
private:
    Bytes script_;

public:
    /// Constructor from script bytes
    /// @param script The invocation script
    explicit InvocationScript(const Bytes& script);

    /// Create from single signature
    /// @param signature The signature
    /// @return The invocation script
    static InvocationScript fromSignature(const Bytes& signature);

    /// Create from multiple signatures
    /// @param signatures The signatures
    /// @return The invocation script
    static InvocationScript fromSignatures(const std::vector<Bytes>& signatures);

    /// Get the script bytes
    /// @return The script
    const Bytes& getScript() const { return script_; }

    /// Get the signatures from the script
    /// @return The signatures
    std::vector<Bytes> getSignatures() const;

    /// Get the number of signatures
    /// @return The signature count
    [[nodiscard]] size_t getSignatureCount() const;
};

} // namespace neocpp
