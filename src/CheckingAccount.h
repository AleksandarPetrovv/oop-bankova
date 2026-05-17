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
    void chargeMonthlyFee();
    double getAvailableBalance() const;

    double getOverdraftLimit() const;
    double getMonthlyFee() const;

    std::string getType() const override;
};
