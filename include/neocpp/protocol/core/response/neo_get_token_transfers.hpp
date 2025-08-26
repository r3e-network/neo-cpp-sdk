#ifndef NEOCPP_PROTOCOL_CORE_RESPONSE_NEO_GET_TOKEN_TRANSFERS_HPP
#define NEOCPP_PROTOCOL_CORE_RESPONSE_NEO_GET_TOKEN_TRANSFERS_HPP

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <neocpp/protocol/core/response.hpp>
#include <neocpp/types/hash160.hpp>
#include <neocpp/types/hash256.hpp>

namespace neocpp {

/**
 * Base interface for token transfer information
 */
class TokenTransfer {
public:
    virtual ~TokenTransfer() = default;
    virtual int getTimestamp() const = 0;
    virtual Hash160 getAssetHash() const = 0;
    virtual std::string getTransferAddress() const = 0;
    virtual int getAmount() const = 0;
    virtual int getBlockIndex() const = 0;
    virtual int getTransferNotifyIndex() const = 0;
    virtual Hash256 getTxHash() const = 0;
};

/**
 * Base interface for token transfers collection
 */
template<typename TTransfer>
class TokenTransfers {
public:
    virtual ~TokenTransfers() = default;
    virtual std::vector<TTransfer> getSent() const = 0;
    virtual std::vector<TTransfer> getReceived() const = 0;
    virtual std::string getTransferAddress() const = 0;
};

/**
 * Generic response type for token transfer queries
 * Used as base for NEP-11 and NEP-17 transfer responses
 */
template<typename T>
class NeoGetTokenTransfers : public Response<T> {
public:
    NeoGetTokenTransfers() = default;
    
    explicit NeoGetTokenTransfers(const nlohmann::json& j) {
        this->from_json(j);
    }
};

} // namespace neocpp

#endif // NEOCPP_PROTOCOL_CORE_RESPONSE_NEO_GET_TOKEN_TRANSFERS_HPP