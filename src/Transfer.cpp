#include "Transfer.h"

#include "Account.h"
#include "Transaction.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

static std::string generateTransferId() {
    static int counter = 1;
    std::ostringstream oss;
    oss << "TRF" << std::setw(5) << std::setfill('0') << counter++;
    return oss.str();
}

Transfer::Transfer(Account* sourceAccount,
                   Account* destAccount,
                   double amount,
                   const std::string& description)
    : sourceAccount(sourceAccount),
      destAccount(destAccount),
      amount(amount),
      description(description),
      transferId(generateTransferId()) {}

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
    if (sourceAccount->getCurrency() != destAccount->getCurrency()) {
        std::cout << "Currency mismatch: " << sourceAccount->getCurrency()
                  << " vs " << destAccount->getCurrency()
                  << ". Cross-currency transfers are not supported.\n";
        return false;
    }

    if (!sourceAccount->transferOut(amount, destAccount, description + " [" + transferId + "]")) {
        return false;
    }
    destAccount->transferIn(amount, sourceAccount, description + " [" + transferId + "]");

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
std::string Transfer::getTransferId() const { return transferId; }
const Transaction* Transfer::getSourceTransaction() const { return sourceTransaction.get(); }
const Transaction* Transfer::getDestTransaction() const { return destTransaction.get(); }
