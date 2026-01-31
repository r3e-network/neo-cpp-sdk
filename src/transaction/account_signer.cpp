
#include "neocpp/transaction/account_signer.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

AccountSigner::AccountSigner(const SharedPtr<Account>& account, WitnessScope scopes)
    : Signer(account->getScriptHash(), scopes), account_(account) {
    if (!account) {
        throw IllegalArgumentException("Account cannot be null");
    }
} // namespace neocpp
SharedPtr<Witness> AccountSigner::createWitness(const Bytes& message) const {
    auto signature = sign(message);

    auto witness = std::make_shared<Witness>();
    witness->setInvocationScript(ScriptBuilder().pushData(signature).toArray());
    witness->setVerificationScript(account_->getVerificationScript());

    return witness;
} // namespace neocpp
Bytes AccountSigner::sign(const Bytes& message) const {
    return account_->sign(message);
} // namespace neocpp
SharedPtr<AccountSigner> AccountSigner::none(const SharedPtr<Account>& account) {
    return std::make_shared<AccountSigner>(account, WitnessScope::NONE);
} // namespace neocpp
SharedPtr<AccountSigner> AccountSigner::calledByEntry(const SharedPtr<Account>& account) {
    return std::make_shared<AccountSigner>(account, WitnessScope::CALLED_BY_ENTRY);
} // namespace neocpp
SharedPtr<AccountSigner> AccountSigner::global(const SharedPtr<Account>& account) {
    return std::make_shared<AccountSigner>(account, WitnessScope::GLOBAL);
} // namespace neocpp
} // namespace neocpp
