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

#include <algorithm>
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
    if (!std::getline(std::cin, s)) {
        std::cout << "\nGoodbye.\n";
        std::exit(0);
    }
    return s;
}

std::string readRequiredLine(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        if (!s.empty()) return s;
        std::cout << "This field is required.\n";
    }
}

int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        if (!std::getline(std::cin, s)) {
            std::cout << "\nGoodbye.\n";
            std::exit(0);
        }
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
        if (!std::getline(std::cin, s)) {
            std::cout << "\nGoodbye.\n";
            std::exit(0);
        }
        std::stringstream ss(s);
        double v;
        if (ss >> v) return v;
        std::cout << "Please enter a number.\n";
    }
}

double readPositiveDouble(const std::string& prompt) {
    while (true) {
        double v = readDouble(prompt);
        if (v > 0) return v;
        std::cout << "Amount must be greater than zero.\n";
    }
}

int readPositiveInt(const std::string& prompt) {
    while (true) {
        int v = readInt(prompt);
        if (v > 0) return v;
        std::cout << "Value must be greater than zero.\n";
    }
}

double readNonNegativeDouble(const std::string& prompt) {
    while (true) {
        double v = readDouble(prompt);
        if (v >= 0) return v;
        std::cout << "Value must be zero or greater.\n";
    }
}

std::string formatTime(std::time_t t, const char* fmt = "%Y-%m-%d %H:%M:%S") {
    char buf[32];
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::strftime(buf, sizeof(buf), fmt, &tm);
    return buf;
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
    std::string name    = readRequiredLine("Name: ");
    std::string email   = readRequiredLine("Email: ");
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
    double initial  = readNonNegativeDouble("Initial deposit: ");
    std::string ccy = readRequiredLine("Currency (e.g. USD): ");

    std::ostringstream id;
    id << "A" << std::setw(4) << std::setfill('0') << g_nextAccountNum++;

    std::shared_ptr<Account> acc;
    if (t == 1) {
        double rate = readNonNegativeDouble("Interest rate (e.g. 0.03 = 3%): ");
        int    lim  = readPositiveInt      ("Monthly withdrawal limit: ");
        acc = std::make_shared<SavingsAccount>(id.str(), ccy, cust.get(), initial, rate, lim);
    } else if (t == 2) {
        double overdraft = readNonNegativeDouble("Overdraft limit: ");
        double fee       = readNonNegativeDouble("Monthly fee: ");
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
    if (!acc->isActive()) { std::cout << "Account is closed.\n"; return; }
    double amount = readPositiveDouble("Amount: ");
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
    if (!acc->isActive()) { std::cout << "Account is closed.\n"; return; }
    double amount = readPositiveDouble("Amount: ");
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
    if (!s->isActive() || !d->isActive()) { std::cout << "One or both accounts are closed.\n"; return; }
    double amount = readPositiveDouble("Amount: ");
    std::string desc = readLine("Description: ");
    Transfer t(s.get(), d.get(), amount, desc.empty() ? "Transfer" : desc);
    if (t.execute()) {
        std::cout << "Transfer complete [" << t.getTransferId() << "]. "
                  << "Source=" << std::fixed << std::setprecision(2) << s->getBalance()
                  << "  Dest=" << d->getBalance() << "\n";
    } else {
        std::cout << "Transfer failed.\n";
    }
}

void doBalance() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }

    std::string openedStr = formatTime(acc->getCreatedAt(), "%Y-%m-%d");

    std::cout << "\n--- Account " << acc->getAccountId() << " ---\n"
              << "Type:     " << acc->getType() << "\n"
              << "Status:   " << (acc->isActive() ? "ACTIVE" : "CLOSED") << "\n"
              << "Currency: " << acc->getCurrency() << "\n"
              << "Opened:   " << openedStr << "\n"
              << "Owner:    " << acc->getOwner()->getName() << "\n"
              << "Balance:  " << std::fixed << std::setprecision(2)
              << acc->getBalance() << " " << acc->getCurrency() << "\n";

    if (acc->getType() == "SAVINGS") {
        auto* sav = dynamic_cast<SavingsAccount*>(acc.get());
        if (sav) {
            std::cout << "Interest rate:        " << sav->getInterestRate() * 100 << "%\n"
                      << "Monthly withdraw limit: " << sav->getMonthlyWithdrawLimit() << "\n"
                      << "Withdrawals this month: " << sav->getWithdrawalsThisMonth() << "\n";
        }
    } else if (acc->getType() == "CHECKING") {
        auto* chk = dynamic_cast<CheckingAccount*>(acc.get());
        if (chk) {
            std::cout << "Overdraft limit: " << chk->getOverdraftLimit() << "\n"
                      << "Monthly fee:     " << chk->getMonthlyFee() << "\n"
                      << "Available (incl. overdraft): " << chk->getAvailableBalance() << "\n";
        }
    }
    std::cout << "Transactions: " << acc->getAllTransactions().size() << "\n";
}

static std::time_t parseDate(const std::string& s) {
    struct tm tm = {};
    int y = 0, m = 0, d = 0;
    if (std::sscanf(s.c_str(), "%d-%d-%d", &y, &m, &d) == 3) {
        tm.tm_year = y - 1900;
        tm.tm_mon  = m - 1;
        tm.tm_mday = d;
        return std::mktime(&tm);
    }
    return (std::time_t)-1;
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

    std::string startS = readLine("Start date YYYY-MM-DD (blank = none): ");
    if (!startS.empty()) {
        std::time_t ts = parseDate(startS);
        if (ts != (std::time_t)-1) f.startDate = ts;
        else std::cout << "Invalid date, ignoring.\n";
    }
    std::string endS = readLine("End date YYYY-MM-DD (blank = none): ");
    if (!endS.empty()) {
        std::time_t ts = parseDate(endS);
        if (ts != (std::time_t)-1) f.endDate = ts;
        else std::cout << "Invalid date, ignoring.\n";
    }

    auto txs = acc->getTransactionHistory(f);
    if (txs.empty()) { std::cout << "(no transactions match filter)\n"; return; }

    std::cout << std::left << std::setw(10) << "TxID"
              << std::setw(14) << "Type"
              << std::setw(12) << "Amount"
              << std::setw(22) << "Date"
              << "Description\n"
              << std::string(70, '-') << "\n";
    for (const auto& tx : txs) {
        std::cout << std::left << std::setw(10) << tx.getTransactionId()
                  << std::setw(14) << Transaction::typeToString(tx.getType())
                  << std::setw(12) << std::fixed << std::setprecision(2) << tx.getAmount()
                  << std::setw(22) << formatTime(tx.getDate())
                  << tx.getDescription() << "\n";
    }
    std::cout << "Total: " << txs.size() << " transaction(s).\n";
}

void doInterest() {
    std::string aid = readLine("Savings account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    if (!acc->isActive()) { std::cout << "Account is closed.\n"; return; }
    auto* sav = dynamic_cast<SavingsAccount*>(acc.get());
    if (!sav) { std::cout << "Not a savings account.\n"; return; }
    double before = sav->getBalance();
    double interest = before * sav->getInterestRate();
    sav->applyInterest();
    std::cout << "Interest applied at " << sav->getInterestRate() * 100 << "%: +"
              << std::fixed << std::setprecision(2) << interest
              << "  " << before << " -> " << sav->getBalance() << "\n";
}

void doResetMonthly() {
    std::string aid = readLine("Savings account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    auto* sav = dynamic_cast<SavingsAccount*>(acc.get());
    if (!sav) { std::cout << "Not a savings account.\n"; return; }
    int before = sav->getWithdrawalsThisMonth();
    sav->resetMonthlyCounter();
    std::cout << "Monthly withdrawal counter reset (" << before << " -> 0).\n";
}

void doStatement() {
    std::string aid = readLine("Account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }

    std::time_t start = acc->getCreatedAt();
    std::time_t end   = std::time(nullptr);

    std::string startS = readLine("Period start YYYY-MM-DD (blank = account open date): ");
    if (!startS.empty()) {
        std::time_t ts = parseDate(startS);
        if (ts != (std::time_t)-1) start = ts;
        else std::cout << "Invalid date, using account open date.\n";
    }
    std::string endS = readLine("Period end YYYY-MM-DD (blank = today): ");
    if (!endS.empty()) {
        std::time_t ts = parseDate(endS);
        if (ts != (std::time_t)-1) end = ts;
        else std::cout << "Invalid date, using today.\n";
    }

    const auto& all = acc->getAllTransactions();
    std::vector<Transaction> inRange;
    for (const auto& tx : all) {
        if (tx.getDate() >= start && tx.getDate() <= end)
            inRange.push_back(tx);
    }

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
    double closing = opening;
    for (const auto& tx : inRange) {
        switch (tx.getType()) {
            case TransactionType::DEPOSIT:
            case TransactionType::TRANSFER_IN:
                closing += tx.getAmount(); break;
            case TransactionType::WITHDRAWAL:
            case TransactionType::TRANSFER_OUT:
                closing -= tx.getAmount(); break;
        }
    }

    Statement s(acc.get(), start, end, opening, closing, inRange);
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

void editCustomer() {
    listCustomers();
    std::string cid = readLine("Customer id to edit: ");
    auto cust = findCustomer(cid);
    if (!cust) { std::cout << "Customer not found.\n"; return; }

    std::cout << "Leave blank to keep current value.\n";
    std::string name = readLine("Name [" + cust->getName() + "]: ");
    if (!name.empty()) cust->setName(name);
    std::string email = readLine("Email [" + cust->getEmail() + "]: ");
    if (!email.empty()) cust->setEmail(email);
    std::string phone = readLine("Phone [" + cust->getPhone() + "]: ");
    if (!phone.empty()) cust->setPhone(phone);
    std::string addr = readLine("Address [" + cust->getAddress() + "]: ");
    if (!addr.empty()) cust->setAddress(addr);
    std::cout << "Customer " << cust->getCustomerId() << " updated.\n";
}

void deleteCustomer() {
    listCustomers();
    std::string cid = readLine("Customer id to delete: ");
    auto it = std::find_if(g_customers.begin(), g_customers.end(),
                           [&cid](const std::shared_ptr<Customer>& c) {
                               return c->getCustomerId() == cid;
                           });
    if (it == g_customers.end()) { std::cout << "Customer not found.\n"; return; }
    if (!(*it)->getAccounts().empty()) {
        std::cout << "Cannot delete customer with open accounts. Close accounts first.\n";
        return;
    }
    std::string confirm = readLine("Type YES to confirm deletion of " + (*it)->getName() + ": ");
    if (confirm != "YES") { std::cout << "Cancelled.\n"; return; }
    g_customers.erase(it);
    std::cout << "Customer " << cid << " deleted.\n";
}

void editAccount() {
    std::string aid = readLine("Account id to edit: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    if (!acc->isActive()) { std::cout << "Account is closed.\n"; return; }

    std::cout << "Leave blank to keep current value.\n";

    if (acc->getType() == "SAVINGS") {
        auto* sav = dynamic_cast<SavingsAccount*>(acc.get());
        std::string rateS = readLine("Interest rate [" + std::to_string(sav->getInterestRate()) + "]: ");
        if (!rateS.empty()) {
            std::stringstream ss(rateS); double v; if (ss >> v && v >= 0) sav->setInterestRate(v);
        }
        std::string limS = readLine("Monthly withdrawal limit [" + std::to_string(sav->getMonthlyWithdrawLimit()) + "]: ");
        if (!limS.empty()) {
            std::stringstream ss(limS); int v; if (ss >> v && v > 0) sav->setMonthlyWithdrawLimit(v);
        }
    } else if (acc->getType() == "CHECKING") {
        auto* chk = dynamic_cast<CheckingAccount*>(acc.get());
        std::string odS = readLine("Overdraft limit [" + std::to_string(chk->getOverdraftLimit()) + "]: ");
        if (!odS.empty()) {
            std::stringstream ss(odS); double v; if (ss >> v && v >= 0) chk->setOverdraftLimit(v);
        }
        std::string feeS = readLine("Monthly fee [" + std::to_string(chk->getMonthlyFee()) + "]: ");
        if (!feeS.empty()) {
            std::stringstream ss(feeS); double v; if (ss >> v && v >= 0) chk->setMonthlyFee(v);
        }
    }
    std::cout << "Account " << acc->getAccountId() << " updated.\n";
}

void closeAccount() {
    std::string aid = readLine("Account id to close: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    if (!acc->isActive()) { std::cout << "Account is already closed.\n"; return; }
    if (acc->getBalance() != 0.0) {
        std::cout << "Balance must be zero before closing. Current balance: "
                  << std::fixed << std::setprecision(2) << acc->getBalance() << "\n";
        return;
    }
    std::string confirm = readLine("Type YES to confirm closing account " + aid + ": ");
    if (confirm != "YES") { std::cout << "Cancelled.\n"; return; }
    acc->close();
    std::cout << "Account " << aid << " closed.\n";
}

void doMonthlyFee() {
    std::string aid = readLine("Checking account id: ");
    auto acc = findAccount(aid);
    if (!acc) { std::cout << "Account not found.\n"; return; }
    if (!acc->isActive()) { std::cout << "Account is closed.\n"; return; }
    auto* chk = dynamic_cast<CheckingAccount*>(acc.get());
    if (!chk) { std::cout << "Not a checking account.\n"; return; }
    double before = chk->getBalance();
    chk->chargeMonthlyFee();
    std::cout << "Monthly fee charged. " << std::fixed << std::setprecision(2)
              << before << " -> " << chk->getBalance() << "\n";
}

void doEndOfMonth() {
    int savReset = 0, chkFees = 0;
    for (auto& c : g_customers) {
        for (auto& a : c->getAccounts()) {
            if (!a->isActive()) continue;
            if (auto* sav = dynamic_cast<SavingsAccount*>(a.get())) {
                sav->resetMonthlyCounter();
                ++savReset;
            } else if (auto* chk = dynamic_cast<CheckingAccount*>(a.get())) {
                chk->chargeMonthlyFee();
                ++chkFees;
            }
        }
    }
    std::cout << "End-of-month batch complete: "
              << savReset << " savings counter(s) reset, "
              << chkFees  << " monthly fee(s) charged.\n";
}

void doListAllAccounts() {
    bool any = false;
    std::cout << std::left << std::setw(8) << "AcctID"
              << std::setw(10) << "Type"
              << std::setw(12) << "Balance"
              << std::setw(6) << "CCY"
              << std::setw(8) << "Status"
              << "Owner\n";
    std::cout << std::string(60, '-') << "\n";
    for (auto& c : g_customers) {
        for (auto& a : c->getAccounts()) {
            any = true;
            std::cout << std::left << std::setw(8) << a->getAccountId()
                      << std::setw(10) << a->getType()
                      << std::setw(12) << std::fixed << std::setprecision(2) << a->getBalance()
                      << std::setw(6) << a->getCurrency()
                      << std::setw(8) << (a->isActive() ? "ACTIVE" : "CLOSED")
                      << c->getName() << "\n";
        }
    }
    if (!any) std::cout << "(no accounts)\n";
}

void seedDemoData() {
    auto c = std::make_shared<Customer>("C0001", "Alice Demo", "alice@example.com",
                                        "+1-555-0100", "1 Demo Lane");
    auto s = std::make_shared<SavingsAccount>("A0001", "USD", c.get(), 1000.0, 0.03, 3);
    auto k = std::make_shared<CheckingAccount>("A0002", "USD", c.get(), 500.0, 200.0, 5.0);

    // Seed sample transactions before observers attach so startup is silent
    s->deposit(500.0, "Initial funding");
    s->deposit(200.0, "Paycheck deposit");
    s->withdraw(150.0, "Grocery shopping");
    k->deposit(300.0, "Cash deposit at branch");
    Transfer seedXfer(s.get(), k.get(), 100.0, "Demo transfer");
    seedXfer.execute();

    s->addObserver(std::make_shared<EmailNotifier>(c->getEmail()));
    s->addObserver(std::make_shared<SmsNotifier>(c->getPhone()));
    k->addObserver(std::make_shared<EmailNotifier>(c->getEmail()));
    k->addObserver(std::make_shared<SmsNotifier>(c->getPhone()));
    c->addAccount(s);
    c->addAccount(k);
    g_customers.push_back(c);

    // Second demo customer for cross-customer transfer testing
    auto c2 = std::make_shared<Customer>("C0002", "Bob Demo", "bob@example.com",
                                         "+1-555-0200", "2 Demo Street");
    auto k2 = std::make_shared<CheckingAccount>("A0003", "USD", c2.get(), 250.0, 100.0, 5.0);
    k2->addObserver(std::make_shared<EmailNotifier>(c2->getEmail()));
    k2->addObserver(std::make_shared<SmsNotifier>(c2->getPhone()));
    c2->addAccount(k2);
    g_customers.push_back(c2);

    g_nextCustomerNum = 3;
    g_nextAccountNum = 4;
}

void printMenu() {
    std::cout << "\n====== OOP Banking System ======\n"
              << " 1) Create customer\n"
              << " 2) Edit customer\n"
              << " 3) Delete customer\n"
              << " 4) List customers / accounts\n"
              << " 5) Open account\n"
              << " 6) Edit account\n"
              << " 7) Close account\n"
              << " 8) List all accounts\n"
              << " 9) Deposit\n"
              << "10) Withdraw\n"
              << "11) Transfer\n"
              << "12) Balance inquiry\n"
              << "13) Transaction history (filter by type/amount/date)\n"
              << "14) Apply interest (savings)\n"
              << "15) Reset monthly withdrawal counter (savings)\n"
              << "16) Charge monthly fee (checking)\n"
              << "17) Generate statement\n"
              << "18) End-of-month batch (reset savings counters + charge all fees)\n"
              << " 0) Exit\n"
              << "================================\n";
}

}  // namespace

int main() {
    seedDemoData();
    std::cout << "OOP Banking System\n"
                 "Pre-loaded demo data:\n"
                 "  C0001 Alice Demo  A0001 savings USD  A0002 checking USD\n"
                 "  C0002 Bob Demo    A0003 checking USD\n"
                 "Use option 4 to browse customers, 13 to view transaction history.\n";

    while (true) {
        printMenu();
        int choice = readInt("Choose: ");
        switch (choice) {
            case 1:  createCustomer();    break;
            case 2:  editCustomer();      break;
            case 3:  deleteCustomer();    break;
            case 4:  doListCustomers();   break;
            case 5:  openAccount();       break;
            case 6:  editAccount();       break;
            case 7:  closeAccount();      break;
            case 8:  doListAllAccounts(); break;
            case 9:  doDeposit();         break;
            case 10: doWithdraw();        break;
            case 11: doTransfer();        break;
            case 12: doBalance();         break;
            case 13: doHistory();         break;
            case 14: doInterest();        break;
            case 15: doResetMonthly();    break;
            case 16: doMonthlyFee();      break;
            case 17: doStatement();       break;
            case 18: doEndOfMonth();      break;
            case 0:
                std::cout << "Goodbye.\n";
                return 0;
            default:
                std::cout << "Unknown choice.\n";
        }
    }
}
