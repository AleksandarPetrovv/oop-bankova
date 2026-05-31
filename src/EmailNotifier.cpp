#include "EmailNotifier.h"

#include "Transaction.h"

#include <iomanip>
#include <iostream>

EmailNotifier::EmailNotifier(const std::string& emailAddress)
    : emailAddress(emailAddress) {}

void EmailNotifier::onTransaction(const Transaction& tx) {
    std::cout << "  [EMAIL -> " << emailAddress << "] "
              << Transaction::typeToString(tx.getType())
              << " | Amount: " << std::fixed << std::setprecision(2) << tx.getAmount()
              << " | Ref: " << tx.getTransactionId()
              << " | " << tx.getDescription() << "\n";
}
