#include "Transfer.h"

#include "Account.h"

#include <iostream>

Transfer::Transfer(Account* sourceAccount,
                   Account* destAccount,
                   double amount,
                   const std::string& description)
    : sourceAccount(sourceAccount),
      destAccount(destAccount),
      amount(amount),
      description(description) {}

bool Transfer::execute() {
    if (!sourceAccount || !destAccount) {
        std::cout << "Transfer needs both source and destination accounts.\n";
        return false;
    }
    if (sourceAccount == destAccount) {
        std::cout << "Cannot transfer to the same account.\n";
        return false;
    }
    if (amount <= 0) {
        std::cout << "Transfer amount must be positive.\n";
        return false;
    }

    if (!sourceAccount->transferOut(amount, destAccount, description)) {
        return false;
    }
    destAccount->transferIn(amount, sourceAccount, description);

    const auto& srcHistory = sourceAccount->getAllTransactions();
    if (!srcHistory.empty()) {
        sourceTransaction = std::make_unique<Transaction>(srcHistory.back());
    }
    const auto& dstHistory = destAccount->getAllTransactions();
    if (!dstHistory.empty()) {
        destTransaction = std::make_unique<Transaction>(dstHistory.back());
    }
    return true;
}

Account* Transfer::getSourceAccount() const { return sourceAccount; }
Account* Transfer::getDestAccount() const { return destAccount; }
double Transfer::getAmount() const { return amount; }
const Transaction* Transfer::getSourceTransaction() const { return sourceTransaction.get(); }
const Transaction* Transfer::getDestTransaction() const { return destTransaction.get(); }
