#pragma once

#include <memory>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include "neocpp/types/types.hpp"

namespace neocpp {

// Forward declaration
class NeoRpcClient;

/// Block polling service for monitoring new blocks
class BlockPolling {
private:
    SharedPtr<NeoRpcClient> rpcClient_;
    std::vector<std::function<void(uint32_t)>> callbacks_;
    std::atomic<bool> running_;
    std::atomic<uint32_t> lastBlockIndex_;
    std::unique_ptr<std::thread> pollingThread_;
    std::chrono::milliseconds pollInterval_;

public:
    /// Constructor
    /// @param rpcClient The RPC client
    /// @param pollInterval The polling interval in milliseconds
    explicit BlockPolling(const SharedPtr<NeoRpcClient>& rpcClient,
                         std::chrono::milliseconds pollInterval = std::chrono::milliseconds(1000));

    /// Destructor
    ~BlockPolling();

    /// Start polling
    void start();

    /// Stop polling
    void stop();

    /// Check if polling is running
    /// @return True if polling
    bool isRunning() const { return running_; }

    /// Subscribe to block updates
    /// @param callback The callback function
    void subscribe(std::function<void(uint32_t)> callback);

    /// Clear all subscriptions
    void clearSubscriptions();

    /// Get last block index
    /// @return The last block index
    [[nodiscard]] uint32_t getLastBlockIndex() const { return lastBlockIndex_; }

    /// Set poll interval
    /// @param interval The interval in milliseconds
    void setPollInterval(std::chrono::milliseconds interval) { pollInterval_ = interval; }

private:
    /// Polling loop
    void pollLoop();

    /// Notify subscribers
    /// @param blockIndex The new block index
    void notifySubscribers(uint32_t blockIndex);
};

} // namespace neocpp
