#include "neocpp/crypto/wif.hpp"
#include "neocpp/utils/base58.hpp"
#include "neocpp/neo_constants.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

// Define static constants
const uint8_t WIF::WIF_VERSION;
const uint8_t WIF::COMPRESSED_FLAG;

std::string WIF::encode(const Bytes& privateKey) {
    if (privateKey.size() != NeoConstants::PRIVATE_KEY_SIZE) {
        throw IllegalArgumentException("Given key is not of expected length (32 bytes)");
    }
    
    Bytes data;
    data.push_back(WIF_VERSION);
    data.insert(data.end(), privateKey.begin(), privateKey.end());
    data.push_back(COMPRESSED_FLAG);
    
    return Base58::encodeCheck(data);
}

Bytes WIF::decode(const std::string& wif) {
    Bytes decoded = Base58::decodeCheck(wif);
    
    if (decoded.empty() || decoded.size() != NeoConstants::PRIVATE_KEY_SIZE + 2 || decoded[0] != WIF_VERSION || decoded[NeoConstants::PRIVATE_KEY_SIZE + 1] != COMPRESSED_FLAG) {
        throw CryptoException("Incorrect WIF format.");
    }
    
    return Bytes(decoded.begin() + 1, decoded.begin() + 1 + NeoConstants::PRIVATE_KEY_SIZE);
}

bool WIF::isValid(const std::string& wif) {
    // We don't check the exact length since Base58 encoding can vary slightly
    // due to leading zeros
    if (wif.empty()) {
        return false;
    }
    
    Bytes decoded = Base58::decodeCheck(wif);
    return !decoded.empty() && 
           decoded.size() == NeoConstants::PRIVATE_KEY_SIZE + 2 && 
           decoded[0] == WIF_VERSION && 
           decoded[NeoConstants::PRIVATE_KEY_SIZE + 1] == COMPRESSED_FLAG;
}

} // namespace neocpp