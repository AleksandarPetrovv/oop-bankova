#include "SavingsAccount.h"

#include <iostream>

SavingsAccount::SavingsAccount(const std::string& accountId,
                               const std::string& currency,
                               Customer* owner,
                               double initialBalance,
                               double interestRate,
                               int monthlyWithdrawLimit)
    : Account(accountId, currency, owner, initialBalance),
      interestRate(interestRate),
      monthlyWithdrawLimit(monthlyWithdrawLimit),
      withdrawalsThisMonth(0) {}

bool SavingsAccount::withdraw(double amount, const std::string& description) {
    if (amount <= 0) {
        std::cout << "Withdrawal amount must be positive.\n";
        return false;
    }
    if (withdrawalsThisMonth >= monthlyWithdrawLimit) {
        std::cout << "Monthly withdrawal limit reached (" << monthlyWithdrawLimit << ").\n";
        return false;
    }
    if (amount > getBalance()) {
        std::cout << "Insufficient funds in savings account.\n";
        return false;
    }
    adjustBalance(-amount);
    ++withdrawalsThisMonth;
    Transaction tx(Transaction::nextId("TXW"), TransactionType::WITHDRAWAL, amount, description);
    recordTransaction(tx);
    return true;
}

bool SavingsAccount::transferOut(double amount, Account* dest, const std::string& description) {
    if (amount <= 0) return false;
    if (withdrawalsThisMonth >= monthlyWithdrawLimit) {
        std::cout << "Monthly withdrawal limit reached (" << monthlyWithdrawLimit << ").\n";
        return false;
    }
    if (amount > getBalance()) {
        std::cout << "Insufficient funds in savings account.\n";
        return false;
    }
    adjustBalance(-amount);
    ++withdrawalsThisMonth;
    Transaction tx(Transaction::nextId("TXO"), TransactionType::TRANSFER_OUT, amount, description, dest);
    recordTransaction(tx);
    return true;
}

void SavingsAccount::applyInterest() {
    double interest = getBalance() * interestRate;
    if (interest <= 0) return;
    adjustBalance(interest);
    Transaction tx(Transaction::nextId("TXI"), TransactionType::DEPOSIT, interest, "Interest applied");
    recordTransaction(tx);
}

void SavingsAccount::resetMonthlyCounter() { withdrawalsThisMonth = 0; }

double SavingsAccount::getInterestRate() const { return interestRate; }
int SavingsAccount::getMonthlyWithdrawLimit() const { return monthlyWithdrawLimit; }
int SavingsAccount::getWithdrawalsThisMonth() const { return withdrawalsThisMonth; }

std::string SavingsAccount::getType() const { return "SAVINGS"; }
