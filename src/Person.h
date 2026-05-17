#pragma once

#include <string>

class Person {
private:
    std::string name;
    std::string email;
    std::string phone;

public:
    Person(const std::string& name, const std::string& email, const std::string& phone);
    virtual ~Person() = default;

    std::string getName() const;
    void setName(const std::string& n);

    std::string getEmail() const;
    void setEmail(const std::string& e);

    std::string getPhone() const;
    void setPhone(const std::string& p);

    virtual std::string getRole() const = 0;
};
