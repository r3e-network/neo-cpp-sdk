# NeoCpp SDK Fixes Design

## Goal
Fix correctness and safety issues in signing, transaction building, witness ordering, fee estimation, scope serialization, and NEP-6 wallet persistence without breaking the public API.

## Scope
- Signing flow: eliminate double-hash bugs by introducing digest-aware signing helpers and using them for transactions.
- Witness/signer ordering: ensure sorted order is applied to transaction state and matching uses correct script-hash endianness.
- Fee estimation: use Neo verification script structure, not Bitcoin-style P2PKH.
- Scope serialization: map witness scopes to JSON correctly, including combined/None.
- NEP-6: honor password for encryption/decryption using NEP-2 and preserve watch-only behavior on failure.
- Tests: re-enable signing tests and add coverage for digest signing, NEP-6 save/load, and ordering.

## Architecture
We keep the public API stable and add digest-aware signing helpers at the crypto layer. Message signing remains “hash then sign,” while transaction signing uses pre-hashed digests. Transaction-related call sites are updated to use digest signing. Witness ordering logic is corrected to mutate the transaction’s internal collections, and witness matching uses the same `Hash160::fromScript` flow as the rest of the codebase to keep endianness consistent. Fee estimation uses the same verification script that real witnesses use (`ScriptBuilder::buildVerificationScript`). Witness scopes are serialized using the existing helper that supports combined scopes.

## Data Flow
- Message signing: raw bytes → SHA-256 → ECDSA.
- Digest signing: 32-byte digest → ECDSA.
- Transaction signing: `doubleSha256(serializeUnsigned)` → digest signing → witness creation.
- Witness ordering: sort signers, then reorder witnesses to match signer account hash derived from verification scripts.
- Fee estimation: use verification script derived from account public key.

## Error Handling
- Digest signing rejects non-32-byte input.
- NEP-6 load: if decryption fails, fall back to watch-only accounts and log debug.
- Fee calculation and scope serialization preserve existing exceptions.

## Testing
- Re-enable and extend crypto signing tests for both message and digest signing.
- Add NEP-6 save/load tests (password vs no password, wrong password fallback).
- Add transaction builder tests for signer/witness sorting and verification script use in fee estimation.
- Add scope serialization tests for “None,” custom scopes, combined scopes.

## Non-Goals
- No sweeping API refactors.
- No changes to external RPC or network behavior beyond the listed fixes.
