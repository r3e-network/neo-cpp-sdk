#include "neocpp/wallet/nep2_error.hpp"

namespace neocpp {
namespace nep2_error {

NEP2Error invalidPassphrase(const std::string& message) {
    return NEP2Error("Invalid passphrase: " + message);
}

NEP2Error invalidFormat(const std::string& message) {
    return NEP2Error("Invalid format: " + message);
}

} // namespace nep2_error
} // namespace neocpp