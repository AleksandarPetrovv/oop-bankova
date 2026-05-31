#pragma once

#include "Transaction.h"

#include <memory>
#include <string>

class Account;

class Transfer {
private:
    Account* sourceAccount;
    Account* destAccount;
    double amount;
    std::string description;
    std::string transferId;
    std::unique_ptr<Transaction> sourceTransaction;
    std::unique_ptr<Transaction> destTransaction;

public:
    Transfer(Account* sourceAccount,
             Account* destAccount,
             double amount,
             const std::string& description = "Transfer");

    bool execute();

    Account* getSourceAccount() const;
    Account* getDestAccount() const;
    double getAmount() const;
    std::string getTransferId() const;
    const Transaction* getSourceTransaction() const;
    const Transaction* getDestTransaction() const;
};
