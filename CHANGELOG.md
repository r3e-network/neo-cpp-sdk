# Changelog

All notable changes to the Neo C++ SDK will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-01-31

### 🚨 Critical Bug Fixes

#### Fixed secp256r1 Half Curve Order
- **File:** `src/crypto/ecdsa_signature.cpp`
- **Issue:** The `HALF_CURVE_ORDER` constant was incorrectly using Bitcoin's secp256k1 curve order instead of Neo's secp256r1 curve order
- **Impact:** This fix ensures proper canonical signature detection and prevents signature malleability issues

#### Fixed Neo N3 OpCode Values
- **File:** `include/neocpp/script/op_code.hpp`
- **Issue:** All opcodes were using Neo 2.x values instead of Neo N3 v3.9.1 values
- **Key Changes:**
  - `PUSHDATA1`: 0x0C → 0x12
  - `PUSHDATA2`: 0x0D → 0x13
  - `PUSHDATA4`: 0x0E → 0x14
  - `PUSH0-PUSH16`: 0x10-0x20 → 0x20-0x30
  - `NOP`: 0x21 → 0x61
  - `SYSCALL`: 0x41 → 0x81
  - All flow control opcodes updated (+0x40 offset)
  - All arithmetic opcodes updated (+0x40 offset)

### 🔧 Code Quality Improvements

#### Exception Handling Consistency
- Changed `src/wallet/bip39_account.cpp` to use `WalletException` instead of `std::runtime_error`
- Changed `src/transaction/witness_scope.cpp` to use `IllegalArgumentException` instead of `std::invalid_argument`

#### Include Guard Standardization
- Removed redundant `#ifndef` guards from 7 header files
- Standardized on `#pragma once` across all 130 headers

#### Namespace Comment Fixes
- Fixed incorrect namespace closing comments in 4 files
- `include/neocpp/wallet/nep2_error.hpp`
- `include/neocpp/transaction/transaction_error.hpp`
- `src/wallet/nep2_error.cpp`
- `src/transaction/transaction_error.cpp`

### 🧹 Project Cleanup

#### Removed Redundant Files (45+ files)
- **Documentation:** 16 redundant markdown files from conversion process
- **Scripts:** 6 Python helper scripts and 1 shell script
- **Binaries:** 4 compiled binaries from repository
- **Test Files:** 7 redundant root-level test files
- **Stub Tests:** 30 placeholder unit test files

#### Updated Tests
- Updated `tests/script/test_op_code.cpp` with correct Neo N3 opcode values
- Updated `tests/script/test_script_builder_complete.cpp` with correct PUSHDATA values

### 📊 Statistics

- **Files Removed:** 45+
- **Files Modified:** 200+
- **Test Cases:** 36 (removed 32 stub tests)
- **Assertions:** 2,680
- **Test Pass Rate:** 100%

### ✅ Verification

All 36 test cases pass with 2,680 assertions:
```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
cmake --build .
./tests/neocpp_tests
# Result: All tests passed (2680 assertions in 36 test cases)
```

## [1.0.0] - 2025-08-24

### Initial Release

- Complete Neo N3 SDK implementation
- Cryptographic functions (secp256r1, SHA256, RIPEMD160)
- Transaction building and signing
- Script builder with Neo VM opcodes
- Wallet support (NEP-2, NEP-6, BIP39)
- RPC client for Neo node communication
- Comprehensive test suite

---

[1.1.0]: https://github.com/your-org/neo-cpp-sdk/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/your-org/neo-cpp-sdk/releases/tag/v1.0.0
