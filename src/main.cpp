#include "Account.h"
#include "CheckingAccount.h"
#include "Customer.h"
#include "EmailNotifier.h"
#include "SavingsAccount.h"
#include "SmsNotifier.h"
#include "Statement.h"
#include "Transaction.h"
#include "TransactionFilter.h"
#include "Transfer.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::vector<std::shared_ptr<Customer>> g_customers;
int g_nextCustomerNum = 1;
int g_nextAccountNum = 1;

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        std::stringstream ss(s);
        int v;
        if (ss >> v) return v;
        std::cout << "Please enter a whole number.\n";
    }
}

double readDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        std::stringstream ss(s);
        double v;
        if (ss >> v) return v;
        std::cout << "Please enter a number.\n";
    }
}

std::shared_ptr<Customer> findCustomer(const std::string& id) {
    for (auto& c : g_customers) {
        if (c->getCustomerId() == id) return c;
    }
    return nullptr;
}

std::shared_ptr<Account> findAccount(const std::string& id) {
    for (auto& c : g_customers) {
        for (auto& a : c->getAccounts()) {
            if (a->getAccountId() == id) return a;
        }
    }
    return nullptr;
}

void listCustomers() {
    if (g_customers.empty()) {
        std::cout << "(no customers yet)\n";
        return;
    }
    for (auto& c : g_customers) {
        std::cout << "  - " << c->getCustomerId()
                  << "  " << c->getName()
                  << "  <" << c->getEmail() << ">"
                  << "  accounts=" << c->getAccounts().size() << "\n";
    }
}

void listAccountsForCustomer(const Customer& c) {
    if (c.getAccounts().empty()) {
        std::cout << "  (no accounts)\n";
        return;
    }
    for (auto& a : c.getAccounts()) {
        std::cout << "  - " << a->getAccountId()
                  << "  " << a->getType()
                  << "  balance=" << std::fixed << std::setprecision(2) << a->getBalance()
                  << " " << a->getCurrency() << "\n";
    }
}

std::shared_ptr<Customer> createCustomer() {
    std::string name    = readLine("Name: ");
    std::string email   = readLine("Email: ");
    std::string phone   = readLine("Phone: ");
    std::string address = readLine("Address: ");
    std::ostringstream id;
    id << "C" << std::setw(4) << std::setfill('0') << g_nextCustomerNum++;
    auto c = std::make_shared<Customer>(id.str(), name, email, phone, address);
    g_customers.push_back(c);
    std::cout << "Created customer " << c->getCustomerId() << ".\n";
    return c;
}

void openAccount() {
    if (g_customers.empty()) {
        std::cout << "Create a customer first.\n";
        return;
    }
    std::cout << "Customers:\n";
    listCustomers();
    std::string cid = readLine("Customer id: ");
    auto cust = findCustomer(cid);
    if (!cust) { std::cout << "Customer not found.\n"; return; }

    std::cout << "Type: 1) Savings  2) Checking\n";
    int t = readInt("> ");
    double initial  = readDouble("Initial deposit: ");
    std::string ccy = readLine("Currency (e.g. USD): ");

    std::ostringstream id;
    id << "A" << std::setw(4) << std::setfill('0') << g_nextAccountNum++;

    std::shared_ptr<Account> acc;
    if (t == 1) {
        double rate = readDouble("Interest rate (e.g. 0.03 = 3%): ");
        int    lim  = readInt   ("Monthly withdrawal limit: ");
        acc = std::make_shared<SavingsAccount>(id.str(), ccy, cust.get(), initial, rate, lim);
    } else if (t == 2) {
        double overdraft = readDouble("Overdraft limit: ");
        double fee       = readDouble("Monthly fee: ");
        acc = std::make_shared<CheckingAccount>(id.str(), ccy, cust.get(), initial, overdraft, fee);
    } else {
        std::cout << "Unknown type.\n";
        return;
    }

    acc->addObserver(std::make_shared<EmailNotifier>(cust->getEmail()));
    acc->addObserver(std::make_shared<SmsNotifier>(cust->getPhone()));
    cust->addAccount(acc);
    std::cout << "Opened account " << acc->getAccountId() << ".\n";
}

void doDeposit() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    double amount = readDouble("Amount: ");
    std::string desc = readLine("Description: ");
    try {
        acc->deposit(amount, desc.empty() ? "Deposit" : desc);
        std::cout << "New balance: " << acc->getBalance() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Deposit failed: " << e.what() << "\n";
    }
}

void doWithdraw() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    double amount = readDouble("Amount: ");
    std::string desc = readLine("Description: ");
    if (acc->withdraw(amount, desc.empty() ? "Withdrawal" : desc)) {
        std::cout << "New balance: " << acc->getBalance() << "\n";
    } else {
        std::cout << "Withdrawal not completed.\n";
    }
}

void doTransfer() {
    std::string src = readLine("Source account id: ");
    std::string dst = readLine("Destination account id: ");
    auto s = findAccount(src);
    auto d = findAccount(dst);
    if (!s || !d) { std::cout << "One of the accounts was not found.\n"; return; }
    double amount = readDouble("Amount: ");
    std::string desc = readLine("Description: ");
    Transfer t(s.get(), d.get(), amount, desc.empty() ? "Transfer" : desc);
    if (t.execute()) {
        std::cout << "Transfer complete. Source=" << s->getBalance()
                  << "  Dest=" << d->getBalance() << "\n";
    } else {
        std::cout << "Transfer failed.\n";
    }
}

void doBalance() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    std::cout << "Balance: " << std::fixed << std::setprecision(2)
              << acc->getBalance() << " " << acc->getCurrency() << "\n";
    if (acc->getType() == "CHECKING") {
        auto* chk = dynamic_cast<CheckingAccount*>(acc.get());
        if (chk) std::cout << "Available (incl. overdraft): " << chk->getAvailableBalance() << "\n";
    }
}

void doHistory() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }

    TransactionFilter f;
    std::cout << "Filter by type? 0=any 1=DEPOSIT 2=WITHDRAWAL 3=TRANSFER_IN 4=TRANSFER_OUT\n";
    int t = readInt("> ");
    switch (t) {
        case 1: f.type = TransactionType::DEPOSIT;      break;
        case 2: f.type = TransactionType::WITHDRAWAL;   break;
        case 3: f.type = TransactionType::TRANSFER_IN;  break;
        case 4: f.type = TransactionType::TRANSFER_OUT; break;
        default: break;
    }
    std::string minS = readLine("Min amount (blank = none): ");
    if (!minS.empty()) { std::stringstream ss(minS); double v; if (ss >> v) f.minAmount = v; }
    std::string maxS = readLine("Max amount (blank = none): ");
    if (!maxS.empty()) { std::stringstream ss(maxS); double v; if (ss >> v) f.maxAmount = v; }

    auto txs = acc->getTransactionHistory(f);
    if (txs.empty()) { std::cout << "(no transactions)\n"; return; }
    for (const auto& tx : txs) {
        std::cout << "  " << tx.getTransactionId()
                  << "  " << Transaction::typeToString(tx.getType())
                  << "  " << std::fixed << std::setprecision(2) << tx.getAmount()
                  << "  " << tx.getDescription() << "\n";
    }
}

void doInterest() {
    std::string aid = readLine("Savings account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    auto* sav = dynamic_cast<SavingsAccount*>(acc.get());
    if (!sav) { std::cout << "Not a savings account.\n"; return; }
    double before = sav->getBalance();
    sav->applyInterest();
    std::cout << "Interest applied. " << before << " -> " << sav->getBalance() << "\n";
}

void doStatement() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    const auto& all = acc->getAllTransactions();
    std::time_t start = acc->getCreatedAt();
    std::time_t end   = std::time(nullptr);
    double opening = acc->getBalance();
    for (const auto& tx : all) {
        switch (tx.getType()) {
            case TransactionType::DEPOSIT:
            case TransactionType::TRANSFER_IN:
                opening -= tx.getAmount(); break;
            case TransactionType::WITHDRAWAL:
            case TransactionType::TRANSFER_OUT:
                opening += tx.getAmount(); break;
        }
    }
    Statement s(acc.get(), start, end, opening, acc->getBalance(), all);
    s.generate();
}

void doListCustomers() {
    listCustomers();
    if (g_customers.empty()) return;
    std::string cid = readLine("Show accounts for customer id (blank to skip): ");
    if (cid.empty()) return;
    auto cust = findCustomer(cid);
    if (!cust) { std::cout << "Customer not found.\n"; return; }
    listAccountsForCustomer(*cust);
}

void seedDemoData() {
    auto c = std::make_shared<Customer>("C0001", "Alice Demo", "alice@example.com",
                                        "+1-555-0100", "1 Demo Lane");
    auto s = std::make_shared<SavingsAccount>("A0001", "USD", c.get(), 1000.0, 0.03, 3);
    auto k = std::make_shared<CheckingAccount>("A0002", "USD", c.get(), 500.0, 200.0, 5.0);
    s->addObserver(std::make_shared<EmailNotifier>(c->getEmail()));
    s->addObserver(std::make_shared<SmsNotifier>(c->getPhone()));
    k->addObserver(std::make_shared<EmailNotifier>(c->getEmail()));
    k->addObserver(std::make_shared<SmsNotifier>(c->getPhone()));
    c->addAccount(s);
    c->addAccount(k);
    g_customers.push_back(c);
    g_nextCustomerNum = 2;
    g_nextAccountNum = 3;
}

void printMenu() {
    std::cout << "\n=========== Bankova ===========\n"
              << " 1) Create customer\n"
              << " 2) Open account\n"
              << " 3) Deposit\n"
              << " 4) Withdraw\n"
              << " 5) Transfer\n"
              << " 6) Balance inquiry\n"
              << " 7) Transaction history (with filter)\n"
              << " 8) Apply interest (savings)\n"
              << " 9) Generate statement\n"
              << "10) List customers / accounts\n"
              << " 0) Exit\n"
              << "================================\n";
}

}  // namespace

int main() {
    seedDemoData();
    std::cout << "Welcome to Bankova. A demo customer C0001 with accounts A0001 (savings) "
                 "and A0002 (checking) has been pre-loaded.\n";

    while (true) {
        printMenu();
        int choice = readInt("Choose: ");
        switch (choice) {
            case 1: createCustomer();   break;
            case 2: openAccount();      break;
            case 3: doDeposit();        break;
            case 4: doWithdraw();       break;
            case 5: doTransfer();       break;
            case 6: doBalance();        break;
            case 7: doHistory();        break;
            case 8: doInterest();       break;
            case 9: doStatement();      break;
            case 10: doListCustomers(); break;
            case 0:
                std::cout << "Goodbye.\n";
                return 0;
            default:
                std::cout << "Unknown choice.\n";
        }
    }
}
