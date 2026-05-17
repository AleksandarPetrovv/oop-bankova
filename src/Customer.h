#pragma once

#include "Person.h"

#include <memory>
#include <string>
#include <vector>

class Account;

class Customer : public Person {
private:
    std::string customerId;
    std::string address;
    std::vector<std::shared_ptr<Account>> accounts;

public:
    Customer(const std::string& customerId,
             const std::string& name,
             const std::string& email,
             const std::string& phone,
             const std::string& address);
    ~Customer() override;

    std::string getCustomerId() const;

    std::string getAddress() const;
    void setAddress(const std::string& a);

    void addAccount(std::shared_ptr<Account> account);
    void removeAccount(const std::string& accountId);
    const std::vector<std::shared_ptr<Account>>& getAccounts() const;

    std::string getRole() const override;
};
