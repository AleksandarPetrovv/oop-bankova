#pragma once

#include "Transaction.h"

#include <ctime>
#include <memory>
#include <string>
#include <vector>

class Customer;
class TransactionObserver;
class TransactionFilter;

class Account {
private:
    double balance;

protected:
    std::string accountId;
    std::string currency;
    Customer* owner;
    std::time_t createdAt;
    std::vector<Transaction> transactions;
    std::vector<std::shared_ptr<TransactionObserver>> observers;

    void recordTransaction(const Transaction& tx);
    void adjustBalance(double delta);

public:
    Account(const std::string& accountId,
            const std::string& currency,
            Customer* owner,
            double initialBalance = 0.0);
    virtual ~Account() = default;

    virtual void deposit(double amount, const std::string& description = "Deposit");
    virtual bool withdraw(double amount, const std::string& description = "Withdrawal") = 0;

    double getBalance() const;
    std::string getAccountId() const;
    std::string getCurrency() const;
    Customer* getOwner() const;
    std::time_t getCreatedAt() const;

    std::vector<Transaction> getTransactionHistory(const TransactionFilter& filter) const;
    const std::vector<Transaction>& getAllTransactions() const;

    void addObserver(std::shared_ptr<TransactionObserver> obs);
    void notifyObservers(const Transaction& tx);

    virtual std::string getType() const = 0;
};
