#pragma once

#include "neocpp/neo_swift_error.hpp"
#include <string>

namespace neocpp {

// Extend the TransactionError class with additional functionality
namespace transaction_error {
    enum class ErrorType {
        SCRIPT_FORMAT,
        SIGNER_CONFIGURATION,
        TRANSACTION_CONFIGURATION
    };

    /**
     * Create a script format error
     * @param message The error message
     * @return A TransactionError with script format error message
     */
    TransactionError scriptFormat(const std::string& message);

    /**
     * Create a signer configuration error
     * @param message The error message
     * @return A TransactionError with signer configuration error message
     */
    TransactionError signerConfiguration(const std::string& message);

    /**
     * Create a transaction configuration error
     * @param message The error message
     * @return A TransactionError with transaction configuration error message
     */
    TransactionError transactionConfiguration(const std::string& message);
}

} // namespace neocpp