#include "Transaction.h"

namespace {
long g_txCounter = 0;
}

Transaction::Transaction(const std::string& transactionId,
                         TransactionType type,
                         double amount,
                         const std::string& description,
                         Account* relatedAccount)
    : transactionId(transactionId),
      type(type),
      amount(amount),
      date(std::time(nullptr)),
      description(description),
      relatedAccount(relatedAccount) {}

std::string Transaction::getTransactionId() const { return transactionId; }
TransactionType Transaction::getType() const { return type; }
double Transaction::getAmount() const { return amount; }
std::time_t Transaction::getDate() const { return date; }
std::string Transaction::getDescription() const { return description; }
Account* Transaction::getRelatedAccount() const { return relatedAccount; }

std::string Transaction::typeToString(TransactionType t) {
    switch (t) {
        case TransactionType::DEPOSIT:      return "DEPOSIT";
        case TransactionType::WITHDRAWAL:   return "WITHDRAWAL";
        case TransactionType::TRANSFER_IN:  return "TRANSFER_IN";
        case TransactionType::TRANSFER_OUT: return "TRANSFER_OUT";
    }
    return "UNKNOWN";
}

std::string Transaction::nextId(const std::string& prefix) {
    return prefix + std::to_string(++g_txCounter);
}
