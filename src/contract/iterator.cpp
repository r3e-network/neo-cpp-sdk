#include "neocpp/contract/iterator.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"

namespace neocpp {

Iterator::Iterator(const std::string& sessionId,
                   const std::string& iteratorId,
                   const SharedPtr<NeoRpcClient>& client,
                   size_t count)
    : sessionId_(sessionId), iteratorId_(iteratorId), client_(client), count_(count), traversed_(false) {
    if (!client) {
        throw IllegalArgumentException("RPC client cannot be null");
    }
}

Iterator::~Iterator() {
    try {
        if (!traversed_) {
            terminate();
        }
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("Error during iterator destruction: ") + e.what());
    } catch (...) {
        LOG_DEBUG("Unknown error during iterator destruction");
    }
}

std::vector<nlohmann::json> Iterator::traverse(size_t count) {
    ensureNotTraversed();
    
    if (count == 0) {
        count = count_;
    }
    
    // Call traverseiterator RPC method
    auto result = client_->traverseIterator(sessionId_, iteratorId_, count);
    traversed_ = true;
    
    std::vector<nlohmann::json> values;
    if (!result.is_null() && result.contains("result") && result["result"].contains("stack")) {
        for (const auto& item : result["result"]["stack"]) {
            values.push_back(item);
        }
    }
    
    return values;
}

void Iterator::terminate() {
    if (!traversed_) {
        try {
            client_->terminateSession(sessionId_);
            traversed_ = true;
        } catch (const std::exception& e) {
            LOG_DEBUG(std::string("Error terminating iterator session: ") + e.what());
        } catch (...) {
            LOG_DEBUG("Unknown error terminating iterator session");
        }
    }
}

void Iterator::ensureNotTraversed() {
    if (traversed_) {
        throw IllegalStateException("Iterator has already been traversed");
    }
}

} // namespace neocpp