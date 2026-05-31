#pragma once

#include "Account.h"

class CheckingAccount : public Account {
private:
    double overdraftLimit;
    double monthlyFee;

public:
    CheckingAccount(const std::string& accountId,
                    const std::string& currency,
                    Customer* owner,
                    double initialBalance,
                    double overdraftLimit,
                    double monthlyFee);

    bool withdraw(double amount, const std::string& description = "Withdrawal") override;
    bool transferOut(double amount, Account* dest, const std::string& description) override;
    void chargeMonthlyFee();
    double getAvailableBalance() const;

    double getOverdraftLimit() const;
    double getMonthlyFee() const;

    void setOverdraftLimit(double limit);
    void setMonthlyFee(double fee);

    std::string getType() const override;
};
