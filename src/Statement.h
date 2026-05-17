#pragma once

#include "Transaction.h"

#include <ctime>
#include <string>
#include <vector>

class Account;

class Statement {
private:
    Account* account;
    std::time_t periodStart;
    std::time_t periodEnd;
    double openingBalance;
    double closingBalance;
    std::vector<Transaction> transactions;

public:
    Statement(Account* account,
              std::time_t periodStart,
              std::time_t periodEnd,
              double openingBalance,
              double closingBalance,
              std::vector<Transaction> transactions);

    void generate() const;
    double getDepositsTotal() const;
    double getWithdrawalsTotal() const;
    int getTransfersCount() const;
};
