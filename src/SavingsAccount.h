#pragma once

#include "Account.h"

class SavingsAccount : public Account {
private:
    double interestRate;
    int monthlyWithdrawLimit;
    int withdrawalsThisMonth;

public:
    SavingsAccount(const std::string& accountId,
                   const std::string& currency,
                   Customer* owner,
                   double initialBalance,
                   double interestRate,
                   int monthlyWithdrawLimit);

    bool withdraw(double amount, const std::string& description = "Withdrawal") override;
    void applyInterest();
    void resetMonthlyCounter();

    double getInterestRate() const;
    int getMonthlyWithdrawLimit() const;
    int getWithdrawalsThisMonth() const;

    std::string getType() const override;
};
