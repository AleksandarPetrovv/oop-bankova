#include "Customer.h"

#include "Account.h"

#include <algorithm>

Customer::Customer(const std::string& customerId,
                   const std::string& name,
                   const std::string& email,
                   const std::string& phone,
                   const std::string& address)
    : Person(name, email, phone), customerId(customerId), address(address) {}

Customer::~Customer() = default;

std::string Customer::getCustomerId() const { return customerId; }

std::string Customer::getAddress() const { return address; }
void Customer::setAddress(const std::string& a) { address = a; }

void Customer::addAccount(std::shared_ptr<Account> account) {
    accounts.push_back(std::move(account));
}

void Customer::removeAccount(const std::string& accountId) {
    accounts.erase(
        std::remove_if(accounts.begin(), accounts.end(),
            [&](const std::shared_ptr<Account>& a) { return a->getAccountId() == accountId; }),
        accounts.end());
}

const std::vector<std::shared_ptr<Account>>& Customer::getAccounts() const {
    return accounts;
}

std::string Customer::getRole() const { return "Customer"; }
