#pragma once

#include <string>
#include <stdexcept>

namespace neocpp {

/// Node plugin types
enum class NodePluginType {
    APPLICATION_LOGS,
    CORE_METRICS,
    IMPORT_BLOCKS,
    LEVEL_DB_STORE,
    ROCKS_DB_STORE,
    RPC_NEP17_TRACKER,
    RPC_SECURITY,
    RPC_SERVER_PLUGIN,
    RPC_SYSTEM_ASSET_TRACKER,
    SIMPLE_POLICY,
    STATES_DUMPER,
    SYSTEM_LOG
};

/// Helper class for NodePluginType operations
class NodePluginTypeHelper {
public:
    /// Convert enum to string value
    static std::string toString(NodePluginType type) {
        switch (type) {
            case NodePluginType::APPLICATION_LOGS: return "ApplicationLogs";
            case NodePluginType::CORE_METRICS: return "CoreMetrics";
            case NodePluginType::IMPORT_BLOCKS: return "ImportBlocks";
            case NodePluginType::LEVEL_DB_STORE: return "LevelDBStore";
            case NodePluginType::ROCKS_DB_STORE: return "RocksDBStore";
            case NodePluginType::RPC_NEP17_TRACKER: return "RpcNep17Tracker";
            case NodePluginType::RPC_SECURITY: return "RpcSecurity";
            case NodePluginType::RPC_SERVER_PLUGIN: return "RpcServerPlugin";
            case NodePluginType::RPC_SYSTEM_ASSET_TRACKER: return "RpcSystemAssetTrackerPlugin";
            case NodePluginType::SIMPLE_POLICY: return "SimplePolicyPlugin";
            case NodePluginType::STATES_DUMPER: return "StatesDumper";
            case NodePluginType::SYSTEM_LOG: return "SystemLog";
            default:
                throw std::invalid_argument("Unknown NodePluginType");
        }
    }

    /// Convert string value to enum
    static NodePluginType fromString(const std::string& value) {
        if (value == "ApplicationLogs") return NodePluginType::APPLICATION_LOGS;
        if (value == "CoreMetrics") return NodePluginType::CORE_METRICS;
        if (value == "ImportBlocks") return NodePluginType::IMPORT_BLOCKS;
        if (value == "LevelDBStore") return NodePluginType::LEVEL_DB_STORE;
        if (value == "RocksDBStore") return NodePluginType::ROCKS_DB_STORE;
        if (value == "RpcNep17Tracker") return NodePluginType::RPC_NEP17_TRACKER;
        if (value == "RpcSecurity") return NodePluginType::RPC_SECURITY;
        if (value == "RpcServerPlugin") return NodePluginType::RPC_SERVER_PLUGIN;
        if (value == "RpcSystemAssetTrackerPlugin") return NodePluginType::RPC_SYSTEM_ASSET_TRACKER;
        if (value == "SimplePolicyPlugin") return NodePluginType::SIMPLE_POLICY;
        if (value == "StatesDumper") return NodePluginType::STATES_DUMPER;
        if (value == "SystemLog") return NodePluginType::SYSTEM_LOG;
        throw std::invalid_argument("Unknown NodePluginType string: " + value);
    }
};

} // namespace neocpp
