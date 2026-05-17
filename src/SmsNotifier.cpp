#include "SmsNotifier.h"

#include "Transaction.h"

#include <iostream>

SmsNotifier::SmsNotifier(const std::string& phoneNumber)
    : phoneNumber(phoneNumber) {}

void SmsNotifier::onTransaction(const Transaction& tx) {
    std::cout << "  [sms -> " << phoneNumber << "] "
              << Transaction::typeToString(tx.getType())
              << " " << tx.getAmount() << "\n";
}
