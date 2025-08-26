#pragma once

#include <string>
#include <optional>
#include <memory>
#include "../types/hash160.hpp"
#include "../neo_swift_error.hpp"

namespace neocpp {

class NeoCpp; // Forward declaration

/**
 * Wrapper class to generate NEP-9 compatible URI schemes for NEP-17 Token transfers.
 * Equivalent to NeoURI in the Swift SDK
 */
class NeoURI {
public:
    static const std::string NEO_SCHEME;
    static const int MIN_NEP9_URI_LENGTH = 38;
    static const std::string NEO_TOKEN_STRING;
    static const std::string GAS_TOKEN_STRING;

    /**
     * Default constructor
     */
    NeoURI();

    /**
     * Constructor with NeoCpp instance
     * @param neoCpp The NeoCpp instance
     */
    explicit NeoURI(std::shared_ptr<NeoCpp> neoCpp);

    /**
     * Creates a NeoURI from a NEP-9 URI String
     * @param uriString A NEP-9 URI String
     * @return A NeoURI object
     * @throws NeoSwiftError if the URI string is invalid
     */
    static NeoURI fromURI(const std::string& uriString);

    /**
     * Sets the recipient's script hash
     * @param recipient The recipient's script hash
     * @return This NeoURI object
     */
    NeoURI& to(const Hash160& recipient);

    /**
     * Sets the token
     * @param token The token hash
     * @return This NeoURI object
     */
    NeoURI& token(const Hash160& token);

    /**
     * Sets the token
     * @param token The token hash, 'neo' or 'gas'
     * @return This NeoURI object
     * @throws NeoSwiftError if the token string is invalid
     */
    NeoURI& token(const std::string& token);

    /**
     * Sets the amount
     * @param amount The amount
     * @return This NeoURI object
     */
    NeoURI& amount(double amount);

    /**
     * Sets the NeoCpp instance
     * @param neoCpp The NeoCpp instance
     * @return This NeoURI object
     */
    NeoURI& neoCpp(std::shared_ptr<NeoCpp> neoCpp);

    /**
     * Builds a NEP-9 URI from the set variables
     * @return This NeoURI object
     * @throws NeoSwiftError if required fields are missing
     */
    NeoURI& buildURI();

    /**
     * Get the NEP-9 URI as string
     * @return The URI string or empty if not built
     */
    std::string getURIString() const;

    /**
     * Get the recipient address
     * @return The recipient address or empty if not set
     */
    std::string getRecipientAddress() const;

    /**
     * Get the token as a string
     * @return The token as a string or empty if not set
     */
    std::string getTokenString() const;

    /**
     * Get the token as an address
     * @return The token as an address or empty if not set
     */
    std::string getTokenAddress() const;

    /**
     * Get the amount as a string
     * @return The amount as a string or empty if not set
     */
    std::string getAmountString() const;

    /**
     * Get the NeoCpp instance
     * @return The NeoCpp instance
     */
    std::shared_ptr<NeoCpp> getNeoCpp() const;

    /**
     * Get the recipient
     * @return The recipient
     */
    const std::optional<Hash160>& getRecipient() const;

    /**
     * Get the token
     * @return The token
     */
    const std::optional<Hash160>& getToken() const;

    /**
     * Get the amount
     * @return The amount
     */
    std::optional<double> getAmount() const;

private:
    std::string uri_;
    std::shared_ptr<NeoCpp> neoCpp_;
    std::optional<Hash160> recipient_;
    std::optional<Hash160> token_;
    std::optional<double> amount_;

    bool isNeoToken(const Hash160& token) const;
    bool isGasToken(const Hash160& token) const;
    std::string buildQueryPart() const;
};

} // namespace neocpp