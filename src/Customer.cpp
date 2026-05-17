#include "Customer.h"

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

void Customer::removeAccount(const std::string& /*accountId*/) {
    // Lookup-by-id added once the Account interface lands in the next commit.
}

const std::vector<std::shared_ptr<Account>>& Customer::getAccounts() const {
    return accounts;
}

std::string Customer::getRole() const { return "Customer"; }
