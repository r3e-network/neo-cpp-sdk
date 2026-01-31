# Neo C++ SDK Fix Identified Issues Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Fix fee estimation for locked/watch-only accounts, serialize signer scopes as JSON arrays, and prevent plaintext WIF persistence by default.

**Architecture:** Store a cached verification script on Account for fee sizing, use array-of-strings for scopes in JSON output, and make NEP-6 JSON omit plaintext keys unless explicitly requested. Changes are localized to wallet/account, transaction builder/signers, and tests.

**Tech Stack:** C++17, Catch2, nlohmann::json.

### Task 1: Scopes JSON array serialization

**Files:**
- Modify: `include/neocpp/transaction/witness_scope.hpp`
- Modify: `src/transaction/transaction_builder.cpp`
- Modify: `src/transaction/signer.cpp`
- Test: `tests/transaction/test_transaction.cpp`
- Test: `tests/transaction/test_signer.cpp`

**Step 1: Write the failing tests**

Update signer JSON expectations to arrays:

```cpp
// tests/transaction/test_transaction.cpp
REQUIRE(signersJson[0]["scopes"].is_array());
REQUIRE(signersJson[0]["scopes"][0] == "None");
REQUIRE(signersJson[2]["scopes"].size() == 2);
REQUIRE(signersJson[2]["scopes"][0] == "CalledByEntry");
REQUIRE(signersJson[2]["scopes"][1] == "CustomContracts");
```

```cpp
// tests/transaction/test_signer.cpp
REQUIRE(json["scopes"].is_array());
REQUIRE(json["scopes"][0] == "CustomContracts");
```

**Step 2: Run test to verify it fails**

Run: `ctest --test-dir build -R neocpp_tests`
Expected: FAIL in signer JSON serialization sections due to string vs array.

**Step 3: Write minimal implementation**

Add helper returning vector of strings from combined scopes and use it in JSON:

```cpp
// include/neocpp/transaction/witness_scope.hpp
static std::vector<std::string> toJsonArray(WitnessScope scope) {
    auto scopes = extractCombinedScopes(static_cast<uint8_t>(scope));
    std::vector<std::string> result;
    result.reserve(scopes.size());
    for (auto s : scopes) {
        result.push_back(toJsonString(s));
    }
    return result;
}
```

```cpp
// src/transaction/transaction_builder.cpp
nlohmann::json scopesJson = nlohmann::json::array();
for (const auto& scopeStr : WitnessScopeHelper::toJsonArray(signer->getScopes())) {
    scopesJson.push_back(scopeStr);
}
```

```cpp
// src/transaction/signer.cpp
nlohmann::json scopesJson = nlohmann::json::array();
for (const auto& scopeStr : WitnessScopeHelper::toJsonArray(scopes_)) {
    scopesJson.push_back(scopeStr);
}
json["scopes"] = scopesJson;
```

**Step 4: Run tests to verify they pass**

Run: `ctest --test-dir build -R neocpp_tests`
Expected: PASS

**Step 5: Commit**

```bash
git add include/neocpp/transaction/witness_scope.hpp src/transaction/transaction_builder.cpp src/transaction/signer.cpp tests/transaction/test_transaction.cpp tests/transaction/test_signer.cpp
git commit -m "fix: serialize signer scopes as json arrays"
```

### Task 2: Fee verification script for locked/watch-only accounts

**Files:**
- Modify: `include/neocpp/wallet/account.hpp`
- Modify: `src/wallet/account.cpp`
- Modify: `src/transaction/transaction_builder.cpp`
- Test: `tests/transaction/test_transaction.cpp`

**Step 1: Write the failing tests**

Add tests for locked and watch-only with explicit script:

```cpp
// tests/transaction/test_transaction.cpp
SECTION("Fee verification script works for locked account") {
    auto account = Account::create();
    auto expected = ScriptBuilder::buildVerificationScript(account->getKeyPair()->getPublicKey());
    account->lock("pw");
    auto feeScript = TransactionBuilder::buildFeeVerificationScript(account);
    REQUIRE(feeScript == expected);
}

SECTION("Fee verification script uses provided script for watch-only") {
    auto watch = Account::fromAddress("NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj");
    auto keyPair = std::make_shared<ECKeyPair>(ECKeyPair::generate());
    auto expected = ScriptBuilder::buildVerificationScript(keyPair->getPublicKey());
    watch->setVerificationScript(expected);
    auto feeScript = TransactionBuilder::buildFeeVerificationScript(watch);
    REQUIRE(feeScript == expected);
}
```

**Step 2: Run test to verify it fails**

Run: `ctest --test-dir build -R neocpp_tests`
Expected: FAIL (throws on missing keypair).

**Step 3: Write minimal implementation**

Cache verification script on Account and use it in fee calculation:

```cpp
// include/neocpp/wallet/account.hpp
private:
    Bytes verificationScript_;
public:
    void setVerificationScript(const Bytes& script) { verificationScript_ = script; }
```

```cpp
// src/wallet/account.cpp (constructors)
verificationScript_ = ScriptBuilder::buildVerificationScript(keyPair_->getPublicKey());
```

```cpp
// src/wallet/account.cpp (NEP2 ctor)
verificationScript_ = ScriptBuilder::buildVerificationScript(tempKeyPair->getPublicKey());
```

```cpp
// src/wallet/account.cpp (multi-sig ctor)
verificationScript_ = ScriptBuilder::buildVerificationScript(publicKeys, signingThreshold);
```

```cpp
// src/wallet/account.cpp
Bytes Account::getVerificationScript() const {
    if (!verificationScript_.empty()) {
        return verificationScript_;
    }
    if (keyPair_) {
        return ScriptBuilder::buildVerificationScript(keyPair_->getPublicKey());
    }
    if (contract_) {
        return contract_->getScript();
    }
    return Bytes();
}
```

```cpp
// src/transaction/transaction_builder.cpp
Bytes script = account ? account->getVerificationScript() : Bytes();
if (script.empty()) {
    throw IllegalArgumentException("Account must provide a verification script for fee calculation");
}
return script;
```

**Step 4: Run tests to verify they pass**

Run: `ctest --test-dir build -R neocpp_tests`
Expected: PASS

**Step 5: Commit**

```bash
git add include/neocpp/wallet/account.hpp src/wallet/account.cpp src/transaction/transaction_builder.cpp tests/transaction/test_transaction.cpp
git commit -m "fix: allow fee scripts from cached verification data"
```

### Task 3: NEP-6 JSON omits plaintext WIF by default

**Files:**
- Modify: `include/neocpp/wallet/nep6_wallet.hpp`
- Modify: `src/wallet/nep6_wallet.cpp`
- Test: `tests/wallet/test_nep6_wallet.cpp`

**Step 1: Write the failing tests**

Add tests for default JSON and explicit plaintext opt-in:

```cpp
SECTION("NEP-6 JSON omits plaintext WIF by default") {
    Nep6Wallet wallet("WIF Test", "1.0");
    wallet.createAccount("Account 1");
    json walletJson = wallet.toJson();
    REQUIRE(walletJson["accounts"][0]["key"].is_null());
}

SECTION("NEP-6 JSON can include plaintext WIF when requested") {
    Nep6Wallet wallet("WIF Test", "1.0");
    wallet.createAccount("Account 1");
    json walletJson = wallet.toJson(true);
    REQUIRE(walletJson["accounts"][0]["key"].is_string());
}
```

**Step 2: Run test to verify it fails**

Run: `ctest --test-dir build -R neocpp_tests`
Expected: FAIL (default JSON includes WIF, overload missing).

**Step 3: Write minimal implementation**

Add overload and use it in save:

```cpp
// include/neocpp/wallet/nep6_wallet.hpp
nlohmann::json toJson(bool includePlaintextWif) const;
```

```cpp
// src/wallet/nep6_wallet.cpp
nlohmann::json Nep6Wallet::toJson() const {
    return toJson(false);
}

nlohmann::json Nep6Wallet::toJson(bool includePlaintextWif) const {
    ...
    if (includePlaintextWif && !account->isLocked() && account->getKeyPair()) {
        accountJson["key"] = account->exportWIF();
    } else if (!account->getEncryptedPrivateKey().empty()) {
        accountJson["key"] = account->getEncryptedPrivateKey();
    } else {
        accountJson["key"] = nullptr;
    }
    ...
}

void Nep6Wallet::save(...) {
    nlohmann::json json = toJson(false);
    ...
}
```

**Step 4: Run tests to verify they pass**

Run: `ctest --test-dir build -R neocpp_tests`
Expected: PASS

**Step 5: Commit**

```bash
git add include/neocpp/wallet/nep6_wallet.hpp src/wallet/nep6_wallet.cpp tests/wallet/test_nep6_wallet.cpp
git commit -m "fix: omit plaintext WIF from nep6 json by default"
```
