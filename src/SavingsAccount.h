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
    bool transferOut(double amount, Account* dest, const std::string& description) override;
    void applyInterest();
    void resetMonthlyCounter();

    double getInterestRate() const;
    int getMonthlyWithdrawLimit() const;
    int getWithdrawalsThisMonth() const;

    void setInterestRate(double rate);
    void setMonthlyWithdrawLimit(int limit);

    std::string getType() const override;
};
