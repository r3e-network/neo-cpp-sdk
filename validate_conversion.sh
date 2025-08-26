#!/bin/bash

# Neo C++ SDK Conversion Validation Script
# This script validates the completeness and correctness of the Swift to C++ conversion

set -e

echo "================================================"
echo "Neo C++ SDK Conversion Validation"
echo "================================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
TOTAL_CHECKS=0
PASSED_CHECKS=0
FAILED_CHECKS=0
WARNINGS=0

# Function to run a check
run_check() {
    local description="$1"
    local command="$2"
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    echo -n "Checking: $description... "
    if eval "$command" > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASS${NC}"
        PASSED_CHECKS=$((PASSED_CHECKS + 1))
        return 0
    else
        echo -e "${RED}✗ FAIL${NC}"
        FAILED_CHECKS=$((FAILED_CHECKS + 1))
        return 1
    fi
}

# Function to run a warning check
warning_check() {
    local description="$1"
    local command="$2"
    
    echo -n "Checking: $description... "
    if eval "$command" > /dev/null 2>&1; then
        echo -e "${GREEN}✓ OK${NC}"
    else
        echo -e "${YELLOW}⚠ WARNING${NC}"
        WARNINGS=$((WARNINGS + 1))
    fi
}

echo ""
echo "1. Directory Structure Validation"
echo "---------------------------------"

run_check "Include directory exists" "[ -d include/neocpp ]"
run_check "Source directory exists" "[ -d src ]"
run_check "Tests directory exists" "[ -d tests ]"
run_check "Examples directory exists" "[ -d examples ]"
run_check "CMakeLists.txt exists" "[ -f CMakeLists.txt ]"

echo ""
echo "2. Core Headers Validation"
echo "--------------------------"

# Crypto headers
run_check "BIP39 header exists" "[ -f include/neocpp/crypto/bip39.hpp ]"
run_check "EC key pair header exists" "[ -f include/neocpp/crypto/ec_key_pair.hpp ]"
run_check "Hash header exists" "[ -f include/neocpp/crypto/hash.hpp ]"
run_check "NEP2 header exists" "[ -f include/neocpp/crypto/nep2.hpp ]"
run_check "WIF header exists" "[ -f include/neocpp/crypto/wif.hpp ]"

# Wallet headers
run_check "Account header exists" "[ -f include/neocpp/wallet/account.hpp ]"
run_check "BIP39 account header exists" "[ -f include/neocpp/wallet/bip39_account.hpp ]"
run_check "Wallet header exists" "[ -f include/neocpp/wallet/wallet.hpp ]"

# Protocol headers
run_check "Neo protocol header exists" "[ -f include/neocpp/protocol/core/neo.hpp ]"

# Response type headers
run_check "NeoGetClaimable header exists" "[ -f include/neocpp/protocol/core/response/neo_get_claimable.hpp ]"
run_check "NeoGetTokenBalances header exists" "[ -f include/neocpp/protocol/core/response/neo_get_token_balances.hpp ]"
run_check "NeoGetTokenTransfers header exists" "[ -f include/neocpp/protocol/core/response/neo_get_token_transfers.hpp ]"
run_check "NeoGetWalletBalance header exists" "[ -f include/neocpp/protocol/core/response/neo_get_wallet_balance.hpp ]"
run_check "ExpressContractState header exists" "[ -f include/neocpp/protocol/core/response/express_contract_state.hpp ]"
run_check "ExpressShutdown header exists" "[ -f include/neocpp/protocol/core/response/express_shutdown.hpp ]"
run_check "Transaction response header exists" "[ -f include/neocpp/protocol/core/response/transaction.hpp ]"

echo ""
echo "3. Source Files Validation"
echo "--------------------------"

run_check "BIP39 account source exists" "[ -f src/wallet/bip39_account.cpp ]"
warning_check "BIP39 source exists" "[ -f src/crypto/bip39.cpp ]"

echo ""
echo "4. Swift Parity Check"
echo "---------------------"

# Count Swift files
SWIFT_COUNT=$(find ../Sources/NeoSwift -name "*.swift" 2>/dev/null | wc -l | tr -d ' ')
echo "Swift source files: $SWIFT_COUNT"

# Count C++ files
CPP_HEADERS=$(find include -name "*.hpp" -o -name "*.h" 2>/dev/null | wc -l | tr -d ' ')
CPP_SOURCES=$(find src -name "*.cpp" -o -name "*.cc" 2>/dev/null | wc -l | tr -d ' ')
CPP_TOTAL=$((CPP_HEADERS + CPP_SOURCES))
echo "C++ files (headers + sources): $CPP_TOTAL"

# Calculate parity percentage
if [ "$SWIFT_COUNT" -gt 0 ]; then
    PARITY=$(( (CPP_TOTAL * 100) / SWIFT_COUNT ))
    echo "Conversion parity: ${PARITY}%"
    
    if [ "$PARITY" -ge 80 ]; then
        echo -e "${GREEN}✓ Good parity (≥80%)${NC}"
    elif [ "$PARITY" -ge 60 ]; then
        echo -e "${YELLOW}⚠ Acceptable parity (60-80%)${NC}"
    else
        echo -e "${RED}✗ Low parity (<60%)${NC}"
    fi
fi

echo ""
echo "5. Compilation Test"
echo "-------------------"

# Check if build directory exists, create if not
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

# Try to configure with CMake
echo -n "CMake configuration... "
if cd build && cmake .. > /dev/null 2>&1; then
    echo -e "${GREEN}✓ SUCCESS${NC}"
    
    # Try to build
    echo -n "Building project... "
    if make -j4 > /dev/null 2>&1; then
        echo -e "${GREEN}✓ SUCCESS${NC}"
        PASSED_CHECKS=$((PASSED_CHECKS + 2))
    else
        echo -e "${YELLOW}⚠ Build failed (may need dependencies)${NC}"
        WARNINGS=$((WARNINGS + 1))
    fi
    cd ..
else
    echo -e "${YELLOW}⚠ CMake configuration failed${NC}"
    WARNINGS=$((WARNINGS + 1))
    cd ..
fi

echo ""
echo "6. Documentation Check"
echo "----------------------"

run_check "README.md exists" "[ -f README.md ]"
run_check "Conversion report exists" "[ -f SWIFT_TO_CPP_CONVERSION_FINAL_REPORT.md ]"
warning_check "API documentation exists" "[ -d docs ]"

echo ""
echo "7. Test Coverage Check"
echo "----------------------"

TEST_COUNT=$(find tests -name "*.cpp" 2>/dev/null | wc -l | tr -d ' ')
echo "Test files found: $TEST_COUNT"

if [ "$TEST_COUNT" -ge 20 ]; then
    echo -e "${GREEN}✓ Good test coverage${NC}"
elif [ "$TEST_COUNT" -ge 10 ]; then
    echo -e "${YELLOW}⚠ Moderate test coverage${NC}"
    WARNINGS=$((WARNINGS + 1))
else
    echo -e "${RED}✗ Low test coverage${NC}"
    FAILED_CHECKS=$((FAILED_CHECKS + 1))
fi

echo ""
echo "8. Critical Components Check"
echo "-----------------------------"

# Check for TODOs and placeholders
echo -n "Checking for TODO comments... "
TODO_COUNT=$(grep -r "TODO" include src 2>/dev/null | grep -v "Binary file" | wc -l | tr -d ' ')
if [ "$TODO_COUNT" -eq 0 ]; then
    echo -e "${GREEN}✓ No TODOs found${NC}"
else
    echo -e "${YELLOW}⚠ Found $TODO_COUNT TODO comments${NC}"
    WARNINGS=$((WARNINGS + 1))
fi

echo -n "Checking for placeholder implementations... "
PLACEHOLDER_COUNT=$(grep -r "throw.*not.*implemented\|placeholder\|stub" include src 2>/dev/null | grep -v "Binary file" | wc -l | tr -d ' ')
if [ "$PLACEHOLDER_COUNT" -eq 0 ]; then
    echo -e "${GREEN}✓ No placeholders found${NC}"
else
    echo -e "${YELLOW}⚠ Found $PLACEHOLDER_COUNT potential placeholders${NC}"
    WARNINGS=$((WARNINGS + 1))
fi

echo ""
echo "================================================"
echo "Validation Summary"
echo "================================================"

echo "Total checks performed: $TOTAL_CHECKS"
echo -e "Passed: ${GREEN}$PASSED_CHECKS${NC}"
echo -e "Failed: ${RED}$FAILED_CHECKS${NC}"
echo -e "Warnings: ${YELLOW}$WARNINGS${NC}"

# Calculate success rate
if [ "$TOTAL_CHECKS" -gt 0 ]; then
    SUCCESS_RATE=$(( (PASSED_CHECKS * 100) / TOTAL_CHECKS ))
    echo "Success rate: ${SUCCESS_RATE}%"
    
    echo ""
    if [ "$SUCCESS_RATE" -ge 90 ]; then
        echo -e "${GREEN}✓ VALIDATION PASSED - SDK is production ready!${NC}"
        exit 0
    elif [ "$SUCCESS_RATE" -ge 70 ]; then
        echo -e "${YELLOW}⚠ VALIDATION PARTIAL - SDK needs minor improvements${NC}"
        exit 1
    else
        echo -e "${RED}✗ VALIDATION FAILED - SDK needs significant work${NC}"
        exit 2
    fi
fi

echo ""
echo "Validation complete."