#include "Account.h"

#include <stdexcept>

class TransactionObserver;
class TransactionFilter;

Account::Account(const std::string& accountId,
                 const std::string& currency,
                 Customer* owner,
                 double initialBalance)
    : balance(initialBalance),
      accountId(accountId),
      currency(currency),
      owner(owner),
      createdAt(std::time(nullptr)) {}

void Account::deposit(double amount, const std::string& description) {
    if (amount <= 0) {
        throw std::invalid_argument("Deposit amount must be positive");
    }
    adjustBalance(amount);
    Transaction tx(Transaction::nextId("TXD"), TransactionType::DEPOSIT, amount, description);
    recordTransaction(tx);
}

double Account::getBalance() const { return balance; }
std::string Account::getAccountId() const { return accountId; }
std::string Account::getCurrency() const { return currency; }
Customer* Account::getOwner() const { return owner; }
std::time_t Account::getCreatedAt() const { return createdAt; }

std::vector<Transaction> Account::getTransactionHistory(const TransactionFilter& /*filter*/) const {
    // TransactionFilter integration arrives with the filter/statement commit.
    return transactions;
}

const std::vector<Transaction>& Account::getAllTransactions() const { return transactions; }

void Account::addObserver(std::shared_ptr<TransactionObserver> obs) {
    observers.push_back(std::move(obs));
}

void Account::notifyObservers(const Transaction& /*tx*/) {
    // Dispatch wired up when TransactionObserver is introduced.
}

void Account::recordTransaction(const Transaction& tx) {
    transactions.push_back(tx);
    notifyObservers(tx);
}

void Account::adjustBalance(double delta) { balance += delta; }
