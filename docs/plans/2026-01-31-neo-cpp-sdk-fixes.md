# NeoCpp SDK Fixes Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Fix signing semantics, transaction ordering, fee estimation, scope serialization, and NEP-6 encryption with test coverage.

**Architecture:** Add digest-aware signing/verification helpers in the crypto layer and update transaction-signing call sites to use them. Correct signer/witness ordering by mutating transaction state in-place, and align fee estimation verification scripts with real Neo verification scripts. Use existing `WitnessScopeHelper` for scope JSON serialization and implement NEP-6 password handling via NEP-2.

**Tech Stack:** C++17, OpenSSL, Catch2, CMake

---

### Task 1: Add digest-aware signing + tests

**Files:**
- Modify: `include/neocpp/crypto/ec_key_pair.hpp:40-105`
- Modify: `src/crypto/ec_key_pair.cpp:128-227`
- Modify: `include/neocpp/crypto/sign.hpp:31-61`
- Modify: `src/crypto/sign.cpp:26-39`
- Test: `tests/crypto/test_sign.cpp:1-120`

**Step 1: Write the failing tests**

Add message-sign and digest-sign sections (and include HashUtils):

```cpp
#include "neocpp/crypto/hash.hpp"

SECTION("Sign message with private key") {
    Bytes privateKeyBytes = Hex::decode(privateKeyHex);
    auto privateKey = std::make_shared<ECPrivateKey>(privateKeyBytes);
    Bytes messageBytes(testMessage.begin(), testMessage.end());

    auto signature = Sign::signMessage(messageBytes, privateKey);
    auto publicKey = privateKey->getPublicKey();

    REQUIRE(signature->getBytes().size() == 64);
    REQUIRE(publicKey->verify(messageBytes, signature));
}

SECTION("Sign hash with private key") {
    Bytes privateKeyBytes = Hex::decode(privateKeyHex);
    auto privateKey = std::make_shared<ECPrivateKey>(privateKeyBytes);
    Bytes messageBytes(testMessage.begin(), testMessage.end());
    Bytes digest = HashUtils::sha256(messageBytes);

    auto signature = Sign::signHash(digest, privateKey);
    auto publicKey = privateKey->getPublicKey();

    REQUIRE(signature->getBytes().size() == 64);
    REQUIRE(publicKey->verifyHash(digest, signature));
}
```

**Step 2: Run test to verify it fails**

Run:

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[crypto]"
```

Expected: compile error (missing `verifyHash`/`signHash` semantics) or failing assertion.

**Step 3: Write minimal implementation**

Add digest-aware methods and fix Sign::signHash:

```cpp
// in ECPrivateKey
SharedPtr<ECDSASignature> signHash(const Bytes& hash) const;

// in ECPublicKey
bool verifyHash(const Bytes& hash, const SharedPtr<ECDSASignature>& signature) const;

// in Sign::signHash
if (hash.size() != 32) throw IllegalArgumentException("Hash must be 32 bytes");
return privateKey->signHash(hash);
```

Implement signHash/verifyHash by skipping SHA-256 and using `ECDSA_do_sign`/`ECDSA_do_verify` on the digest directly.

**Step 4: Run test to verify it passes**

Run:

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[crypto]"
```

Expected: PASS.

**Step 5: Commit**

```bash
git add include/neocpp/crypto/ec_key_pair.hpp src/crypto/ec_key_pair.cpp include/neocpp/crypto/sign.hpp src/crypto/sign.cpp tests/crypto/test_sign.cpp
git commit -m "fix: add digest signing helpers"
```

---

### Task 2: Use digest signing for transactions

**Files:**
- Modify: `include/neocpp/wallet/account.hpp:96-115`
- Modify: `src/wallet/account.cpp:100-119`
- Modify: `src/transaction/transaction.cpp:44-53`
- Modify: `src/transaction/transaction_builder.cpp:332-343`
- Modify: `src/wallet/wallet.cpp:109-121`
- Test: `tests/transaction/test_transaction_builder_complete.cpp:180-240`

**Step 1: Write the failing test**

Add a section verifying witness signatures match the transaction hash digest:

```cpp
SECTION("Transaction signatures use digest") {
    TransactionBuilder builder(mockClient);
    builder.script({0x51})
           .signers({AccountSigner::calledByEntry(account1)});

    auto tx = builder.buildAndSign();
    REQUIRE(tx->getWitnesses().size() == 1);

    const auto& invocation = tx->getWitnesses()[0]->getInvocationScript();
    REQUIRE(invocation.size() == 1 + 64);
    uint8_t len = invocation[0];
    REQUIRE(len == 64);

    Bytes sigBytes(invocation.begin() + 1, invocation.begin() + 1 + len);
    auto sig = std::make_shared<ECDSASignature>(sigBytes);
    auto pubKey = account1->getKeyPair()->getPublicKey();

    REQUIRE(pubKey->verifyHash(tx->getHash().toArray(), sig));
}
```

**Step 2: Run test to verify it fails**

Run:

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[transaction]"
```

Expected: FAIL (signature verifies against hashed message instead of digest).

**Step 3: Write minimal implementation**

Add `Account::signHash` and use it in transaction signing:

```cpp
// Account
Bytes signHash(const Bytes& hash) const;

// Call sites
account->signHash(tx->getHash().toArray());
```

Update:
- `Transaction::sign` to sign `getHash()` with `signHash`.
- `TransactionBuilder::addWitnessForAccount` to use `account->signHash(txHash.toArray())`.
- `Wallet::signTransaction` to use `transaction->getHash()` and `account->signHash` (no `getHashData()`).

**Step 4: Run test to verify it passes**

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[transaction]"
```

Expected: PASS.

**Step 5: Commit**

```bash
git add include/neocpp/wallet/account.hpp src/wallet/account.cpp src/transaction/transaction.cpp src/transaction/transaction_builder.cpp src/wallet/wallet.cpp tests/transaction/test_transaction_builder_complete.cpp
git commit -m "fix: sign transactions with digest"
```

---

### Task 3: Fix signer/witness ordering

**Files:**
- Modify: `include/neocpp/transaction/transaction.hpp:48-70`
- Modify: `src/transaction/transaction.cpp:41-43`
- Modify: `src/transaction/transaction_builder.cpp:346-383`
- Test: `tests/transaction/test_transaction_builder_complete.cpp:330-380`

**Step 1: Write the failing test**

Update ordering test and add witness ordering check:

```cpp
SECTION("Multiple signers ordering") {
    TransactionBuilder builder(mockClient);
    auto account3 = Account::create();

    builder.script({0x51})
           .signers({
               AccountSigner::none(account2),
               AccountSigner::none(account1),
               AccountSigner::none(account3)
           });

    auto tx = builder.build();
    auto signers = tx->getSigners();
    REQUIRE(signers.size() == 3);

    std::vector<Hash160> expected = {
        account2->getScriptHash(),
        account1->getScriptHash(),
        account3->getScriptHash()
    };
    std::sort(expected.begin(), expected.end(), [](const Hash160& a, const Hash160& b) {
        return a.toString() < b.toString();
    });

    REQUIRE(signers[0]->getAccount() == expected[0]);
    REQUIRE(signers[1]->getAccount() == expected[1]);
    REQUIRE(signers[2]->getAccount() == expected[2]);
}

SECTION("Witness ordering matches signers") {
    TransactionBuilder builder(mockClient);
    builder.script({0x51})
           .signers({
               AccountSigner::none(account2),
               AccountSigner::none(account1)
           });

    auto tx = builder.buildAndSign();
    auto signers = tx->getSigners();
    auto witnesses = tx->getWitnesses();

    REQUIRE(signers.size() == witnesses.size());
    for (size_t i = 0; i < signers.size(); ++i) {
        Hash160 witnessHash = Hash160::fromScript(witnesses[i]->getVerificationScript());
        REQUIRE(witnessHash == signers[i]->getAccount());
    }
}
```

**Step 2: Run test to verify it fails**

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[transaction]"
```

Expected: FAIL (signers not reordered, witnesses not aligned).

**Step 3: Write minimal implementation**

- Add `clearWitnesses()` to `Transaction`.
- In `TransactionBuilder::sortSigners`, sort then `clearSigners()` and re-add sorted signers.
- In `TransactionBuilder::sortWitnesses`, compute `Hash160::fromScript(verificationScript)` and clear/re-add witnesses in signer order.

**Step 4: Run test to verify it passes**

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[transaction]"
```

Expected: PASS.

**Step 5: Commit**

```bash
git add include/neocpp/transaction/transaction.hpp src/transaction/transaction.cpp src/transaction/transaction_builder.cpp tests/transaction/test_transaction_builder_complete.cpp
git commit -m "fix: sort signers and witnesses deterministically"
```

---

### Task 4: Fix scope serialization + fee verification script

**Files:**
- Modify: `src/transaction/transaction_builder.cpp:262-272`
- Modify: `src/transaction/transaction_builder.cpp:657-690`

**Step 1: Write the failing test**

No isolated unit test available (RPC client is concrete and not mockable without network). Proceed with code change and rely on existing transaction tests.

**Step 2: Implement minimal changes**

- Use `WitnessScopeHelper::toJsonString` in `callInvokeScript`.
- Replace P2PKH script in `createFakeVerificationScript` with:

```cpp
return ScriptBuilder::buildVerificationScript(account->getKeyPair()->getPublicKey());
```

**Step 3: Run focused tests**

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[transaction]"
```

Expected: PASS.

**Step 4: Commit**

```bash
git add src/transaction/transaction_builder.cpp
git commit -m "fix: scope serialization and fee script"
```

---

### Task 5: Implement NEP-6 encryption/decryption

**Files:**
- Modify: `src/wallet/nep6_wallet.cpp:14-151`
- Test: `tests/wallet/test_nep6_wallet.cpp:140-260`

**Step 1: Write the failing test**

Strengthen the encrypted wallet test to assert NEP-2 keys and unlock behavior:

```cpp
SECTION("Save and load encrypted NEP-6 wallet") {
    std::string filepath = "/tmp/test_encrypted_nep6_wallet.json";
    std::string password = "StrongPassword123!";

    {
        Nep6Wallet wallet("Encrypted NEP-6", "1.0");
        wallet.createAccount("Secure Account");
        wallet.save(filepath, password);
    }

    // Verify saved key is NEP-2
    {
        std::ifstream file(filepath);
        json walletJson;
        file >> walletJson;
        auto key = walletJson["accounts"][0]["key"].get<std::string>();
        REQUIRE(key.size() == 58);
    }

    // Wrong password -> watch-only
    {
        auto wrongWallet = Nep6Wallet::load(filepath, "WrongPassword");
        REQUIRE(wrongWallet->size() == 1);
        auto acct = wrongWallet->getDefaultAccount();
        REQUIRE(acct != nullptr);
        REQUIRE(acct->getKeyPair() == nullptr);
    }

    // Correct password -> unlock succeeds
    {
        auto correctWallet = Nep6Wallet::load(filepath, password);
        auto acct = correctWallet->getDefaultAccount();
        REQUIRE(acct != nullptr);
        REQUIRE(acct->unlock(password));
    }

    std::remove(filepath.c_str());
}
```

**Step 2: Run test to verify it fails**

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[wallet]"
```

Expected: FAIL (keys saved as WIF, password ignored).

**Step 3: Write minimal implementation**

- In `Nep6Wallet::save`, if `password` is non-empty and account is unlocked, use `exportNEP2(password)`.
- In `fromJson`, honor `password` and remove `(void)password`.
- Preserve existing fallback to watch-only on decryption failure.

**Step 4: Run test to verify it passes**

```bash
cmake --build build -j4
./build/tests/neocpp_tests "[wallet]"
```

Expected: PASS.

**Step 5: Commit**

```bash
git add src/wallet/nep6_wallet.cpp tests/wallet/test_nep6_wallet.cpp
git commit -m "fix: honor NEP-6 password encryption"
```

---

### Task 6: Full regression run

**Files:**
- Test: `build/tests/neocpp_tests`

**Step 1: Run full test suite**

```bash
cmake --build build -j4
ctest --test-dir build
```

Expected: PASS.

**Step 2: Commit any remaining adjustments**

```bash
git status --short
# if clean, no commit needed
```
