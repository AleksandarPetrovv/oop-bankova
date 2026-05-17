#include "EmailNotifier.h"

#include "Transaction.h"

#include <iostream>

EmailNotifier::EmailNotifier(const std::string& emailAddress)
    : emailAddress(emailAddress) {}

void EmailNotifier::onTransaction(const Transaction& tx) {
    std::cout << "  [email -> " << emailAddress << "] "
              << Transaction::typeToString(tx.getType())
              << " of " << tx.getAmount()
              << " (" << tx.getDescription() << ")\n";
}
