#pragma once

#include <string>
#include <vector>
#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/types/contract_parameter_type.hpp"

namespace neocpp {

/// Represents a smart contract
class Contract {
private:
    Bytes script_;
    std::vector<ContractParameterType> parameterList_;
    bool deployed_;

public:
    /// Default constructor
    Contract() : deployed_(false) {}

    /// Construct with script
    /// @param script The contract script
    explicit Contract(const Bytes& script)
        : script_(script), deployed_(false) {}

    /// Construct with script and parameter list
    /// @param script The contract script
    /// @param parameterList The parameter types
    Contract(const Bytes& script, const std::vector<ContractParameterType>& parameterList)
        : script_(script), parameterList_(parameterList), deployed_(false) {}

    /// Get the contract script
    /// @return The script bytes
    const Bytes& getScript() const { return script_; }

    /// Set the contract script
    /// @param script The script bytes
    void setScript(const Bytes& script) { script_ = script; }

    /// Get the parameter list
    /// @return The parameter types
    const std::vector<ContractParameterType>& getParameterList() const { return parameterList_; }

    /// Set the parameter list
    /// @param parameterList The parameter types
    void setParameterList(const std::vector<ContractParameterType>& parameterList) {
        parameterList_ = parameterList;
    }

    /// Check if contract is deployed
    /// @return True if deployed
    bool isDeployed() const { return deployed_; }

    /// Set deployed status
    /// @param deployed The deployed status
    void setDeployed(bool deployed) { deployed_ = deployed; }
};

} // namespace neocpp
