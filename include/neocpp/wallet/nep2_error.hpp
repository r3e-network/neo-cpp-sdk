#pragma once

#include "../neo_swift_error.hpp"

namespace neocpp {

// Helper namespace for NEP2Error factory functions
namespace nep2_error {
    /**
     * Create an invalid passphrase error
     * @param message The error message
     * @return A NEP2Error with invalid passphrase message
     */
    NEP2Error invalidPassphrase(const std::string& message);

    /**
     * Create an invalid format error
     * @param message The error message
     * @return A NEP2Error with invalid format message
     */
    NEP2Error invalidFormat(const std::string& message);
}

} // namespace neocpp