#pragma once

// Main header file for NeoCpp SDK
// Include all necessary components

// Core types
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/hash256.hpp"
#include "neocpp/types/call_flags.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/types/contract_parameter_type.hpp"
#include "neocpp/types/neo_vm_state_type.hpp"
#include "neocpp/types/node_plugin_type.hpp"

// Constants
#include "neocpp/neo_constants.hpp"

// Crypto
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ec_point.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/crypto/sign.hpp"
#include "neocpp/crypto/wif.hpp"
#include "neocpp/crypto/nep2.hpp"
#include "neocpp/crypto/scrypt_params.hpp"
#include "neocpp/crypto/bip32_ec_key_pair.hpp"

// Wallet
#include "neocpp/wallet/wallet.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/wallet/nep6_wallet.hpp"
#include "neocpp/wallet/nep6_account.hpp"

// Transaction
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/signer.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/transaction/witness_scope.hpp"
#include "neocpp/transaction/witness_rule.hpp"
#include "neocpp/transaction/transaction_attribute.hpp"

// Script
#include "neocpp/script/script_builder.hpp"
#include "neocpp/script/op_code.hpp"

// Serialization
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

// Protocol/RPC
#include "neocpp/protocol/neo_cpp.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/protocol/http_service.hpp"
#include "neocpp/protocol/response_types_impl.hpp"
#include "neocpp/protocol/stack_item.hpp"

// Utils
#include "neocpp/utils/base58.hpp"
#include "neocpp/utils/base64.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/utils/address.hpp"

// Exceptions
#include "neocpp/exceptions.hpp"

namespace neocpp {
    // Version information
    constexpr const char* VERSION = "1.0.0";
    constexpr int VERSION_MAJOR = 1;
    constexpr int VERSION_MINOR = 0;
    constexpr int VERSION_PATCH = 0;
} // namespace neocpp
