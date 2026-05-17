#pragma once

#include <ctime>
#include <string>

class Account;

enum class TransactionType {
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER_IN,
    TRANSFER_OUT
};

class Transaction {
private:
    std::string transactionId;
    TransactionType type;
    double amount;
    std::time_t date;
    std::string description;
    Account* relatedAccount;

public:
    Transaction(const std::string& transactionId,
                TransactionType type,
                double amount,
                const std::string& description,
                Account* relatedAccount = nullptr);

    std::string getTransactionId() const;
    TransactionType getType() const;
    double getAmount() const;
    std::time_t getDate() const;
    std::string getDescription() const;
    Account* getRelatedAccount() const;

    static std::string typeToString(TransactionType t);
    static std::string nextId(const std::string& prefix);
};
