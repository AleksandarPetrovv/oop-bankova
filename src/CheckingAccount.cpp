#include "CheckingAccount.h"

#include <iostream>

CheckingAccount::CheckingAccount(const std::string& accountId,
                                 const std::string& currency,
                                 Customer* owner,
                                 double initialBalance,
                                 double overdraftLimit,
                                 double monthlyFee)
    : Account(accountId, currency, owner, initialBalance),
      overdraftLimit(overdraftLimit),
      monthlyFee(monthlyFee) {}

bool CheckingAccount::withdraw(double amount, const std::string& description) {
    if (amount <= 0) {
        std::cout << "Withdrawal amount must be positive.\n";
        return false;
    }
    if (amount > getBalance() + overdraftLimit) {
        std::cout << "Withdrawal exceeds overdraft limit.\n";
        return false;
    }
    adjustBalance(-amount);
    Transaction tx(Transaction::nextId("TXW"), TransactionType::WITHDRAWAL, amount, description);
    recordTransaction(tx);
    return true;
}

void CheckingAccount::chargeMonthlyFee() {
    if (monthlyFee <= 0) return;
    adjustBalance(-monthlyFee);
    Transaction tx(Transaction::nextId("TXF"), TransactionType::WITHDRAWAL, monthlyFee, "Monthly maintenance fee");
    recordTransaction(tx);
}

double CheckingAccount::getAvailableBalance() const { return getBalance() + overdraftLimit; }
double CheckingAccount::getOverdraftLimit() const { return overdraftLimit; }
double CheckingAccount::getMonthlyFee() const { return monthlyFee; }

std::string CheckingAccount::getType() const { return "CHECKING"; }
