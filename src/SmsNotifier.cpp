#include "SmsNotifier.h"

#include "Transaction.h"

#include <iomanip>
#include <iostream>

SmsNotifier::SmsNotifier(const std::string& phoneNumber)
    : phoneNumber(phoneNumber) {}

void SmsNotifier::onTransaction(const Transaction& tx) {
    std::cout << "  [SMS -> " << phoneNumber << "] "
              << Transaction::typeToString(tx.getType())
              << " " << std::fixed << std::setprecision(2) << tx.getAmount()
              << " [" << tx.getTransactionId() << "]\n";
}
