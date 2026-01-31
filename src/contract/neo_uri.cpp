#include "neocpp/contract/neo_uri.hpp"
#include "neocpp/contract/neo_token.hpp"
#include "neocpp/contract/gas_token.hpp"
#include "neocpp/neo_swift_error.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"
#include <sstream>
#include <vector>

namespace neocpp {

const std::string NeoURI::NEO_SCHEME = "neo";
const std::string NeoURI::NEO_TOKEN_STRING = "neo";
const std::string NeoURI::GAS_TOKEN_STRING = "gas";

NeoURI::NeoURI() = default;

NeoURI::NeoURI(std::shared_ptr<NeoCpp> neoCpp) : neoCpp_(std::move(neoCpp)) {}

NeoURI NeoURI::fromURI(const std::string& uriString) {
    // Check minimum length and scheme
    if (uriString.length() < MIN_NEP9_URI_LENGTH || uriString.substr(0, 4) != "neo:") {
        throw ValidationError("The provided string does not conform to the NEP-9 standard.");
    }

    // Split into base and query parts
    size_t queryPos = uriString.find('?');
    std::string base = queryPos != std::string::npos ? uriString.substr(0, queryPos) : uriString;
    
    // Extract address
    std::string address = base.substr(4); // Skip "neo:"
    
    // Create URI with recipient
    NeoURI neoURI;
    neoURI.to(Hash160::fromAddress(address));
    
    // Process query parameters if present
    if (queryPos != std::string::npos) {
        std::string query = uriString.substr(queryPos + 1);
        std::vector<std::string> params;
        
        // Split query by &
        size_t start = 0;
        size_t end = query.find('&');
        while (end != std::string::npos) {
            params.push_back(query.substr(start, end - start));
            start = end + 1;
            end = query.find('&', start);
        }
        params.push_back(query.substr(start));
        
        // Process each parameter
        for (const auto& param : params) {
            size_t equalPos = param.find('=');
            if (equalPos == std::string::npos) {
                throw ValidationError("This URI contains invalid queries.");
            }
            
            std::string key = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            
            if (key == "asset" && !neoURI.token_) {
                neoURI.token(value);
            } else if (key == "amount" && !neoURI.amount_) {
                try {
                    neoURI.amount(std::stod(value));
                } catch (const std::exception&) {
                    // Ignore invalid amount
                }
            }
        }
    }
    
    return neoURI;
}

NeoURI& NeoURI::to(const Hash160& recipient) {
    recipient_ = recipient;
    return *this;
}

NeoURI& NeoURI::token(const Hash160& token) {
    token_ = token;
    return *this;
}

NeoURI& NeoURI::token(const std::string& token) {
    if (token == NEO_TOKEN_STRING) {
        token_ = NeoToken::SCRIPT_HASH;
    } else if (token == GAS_TOKEN_STRING) {
        token_ = GasToken::SCRIPT_HASH;
    } else {
        token_ = Hash160(token);
    }
    return *this;
}

NeoURI& NeoURI::amount(double amount) {
    amount_ = amount;
    return *this;
}

NeoURI& NeoURI::neoCpp(std::shared_ptr<NeoCpp> neoCpp) {
    neoCpp_ = std::move(neoCpp);
    return *this;
}

NeoURI& NeoURI::buildURI() {
    if (!recipient_) {
        throw ValidationError("Could not create a NEP-9 URI without a recipient address.");
    }
    
    std::string base = NEO_SCHEME + ":" + recipient_->toAddress();
    std::string query = buildQueryPart();
    
    uri_ = base + (query.empty() ? "" : "?" + query);
    return *this;
}

std::string NeoURI::getURIString() const {
    return uri_;
}

std::string NeoURI::getRecipientAddress() const {
    return recipient_ ? recipient_->toAddress() : "";
}

std::string NeoURI::getTokenString() const {
    if (!token_) {
        return "";
    }
    
    if (isNeoToken(*token_)) {
        return NEO_TOKEN_STRING;
    } else if (isGasToken(*token_)) {
        return GAS_TOKEN_STRING;
    } else {
        return token_->toString();
    }
}

std::string NeoURI::getTokenAddress() const {
    return token_ ? token_->toAddress() : "";
}

std::string NeoURI::getAmountString() const {
    if (!amount_) {
        return "";
    }
    
    std::ostringstream ss;
    ss << *amount_;
    return ss.str();
}

std::shared_ptr<NeoCpp> NeoURI::getNeoCpp() const {
    return neoCpp_;
}

const std::optional<Hash160>& NeoURI::getRecipient() const {
    return recipient_;
}

const std::optional<Hash160>& NeoURI::getToken() const {
    return token_;
}

std::optional<double> NeoURI::getAmount() const {
    return amount_;
}

bool NeoURI::isNeoToken(const Hash160& token) const {
    return token == NeoToken::SCRIPT_HASH;
}

bool NeoURI::isGasToken(const Hash160& token) const {
    return token == GasToken::SCRIPT_HASH;
}

std::string NeoURI::buildQueryPart() const {
    std::vector<std::string> query;
    
    if (token_) {
        if (isNeoToken(*token_)) {
            query.push_back("asset=" + NEO_TOKEN_STRING);
        } else if (isGasToken(*token_)) {
            query.push_back("asset=" + GAS_TOKEN_STRING);
        } else {
            query.push_back("asset=" + token_->toString());
        }
    }
    
    if (amount_) {
        std::ostringstream ss;
        ss << *amount_;
        query.push_back("amount=" + ss.str());
    }
    
    if (query.empty()) {
        return "";
    }
    
    std::string result = query[0];
    for (size_t i = 1; i < query.size(); i++) {
        result += "&" + query[i];
    }
    
    return result;
}

} // namespace neocpp