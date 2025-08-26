#include "neocpp/transaction/transaction_error.hpp"

namespace neocpp {
namespace transaction_error {

TransactionError scriptFormat(const std::string& message) {
    return TransactionError("Script format error: " + message);
}

TransactionError signerConfiguration(const std::string& message) {
    return TransactionError("Signer configuration error: " + message);
}

TransactionError transactionConfiguration(const std::string& message) {
    return TransactionError("Transaction configuration error: " + message);
}

} // namespace transaction_error
} // namespace neocpp