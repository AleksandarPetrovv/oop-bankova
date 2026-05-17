#include "Person.h"

Person::Person(const std::string& name, const std::string& email, const std::string& phone)
    : name(name), email(email), phone(phone) {}

std::string Person::getName() const { return name; }
void Person::setName(const std::string& n) { name = n; }

std::string Person::getEmail() const { return email; }
void Person::setEmail(const std::string& e) { email = e; }

std::string Person::getPhone() const { return phone; }
void Person::setPhone(const std::string& p) { phone = p; }
