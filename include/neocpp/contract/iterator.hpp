#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

// Forward declarations
class NeoRpcClient;

/// Iterator for traversing contract storage or results
class Iterator {
private:
    std::string sessionId_;
    std::string iteratorId_;
    SharedPtr<NeoRpcClient> client_;
    size_t count_;
    bool traversed_;

public:
    /// Constructor
    /// @param sessionId The RPC session ID
    /// @param iteratorId The iterator ID
    /// @param client The RPC client
    /// @param count The number of items
    Iterator(const std::string& sessionId,
             const std::string& iteratorId,
             const SharedPtr<NeoRpcClient>& client,
             size_t count = 0);

    /// Destructor
    ~Iterator();

    /// Get session ID
    const std::string& getSessionId() const { return sessionId_; }

    /// Get iterator ID
    const std::string& getIteratorId() const { return iteratorId_; }

    /// Get count
    [[nodiscard]] size_t getCount() const { return count_; }

    /// Check if traversed
    bool isTraversed() const { return traversed_; }

    /// Traverse iterator and get values
    /// @param count Number of items to get (0 for all)
    /// @return The values
    std::vector<nlohmann::json> traverse(size_t count = 0);

    /// Terminate the iterator
    void terminate();

private:
    /// Ensure session is valid
    void ensureNotTraversed();
};

} // namespace neocpp
