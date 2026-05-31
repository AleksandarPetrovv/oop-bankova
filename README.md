# OOP Banking System (C++17)

A console-based banking system demonstrating OOP principles including inheritance, polymorphism, and the Observer design pattern.

## Build

```sh
g++ -std=c++17 src/*.cpp -o bank
./bank          # Linux/macOS
bank.exe        # Windows
```

Requires g++ with C++17 support.

## Features

| # | Feature | Details |
|---|---------|---------|
| 1 | Customer management | Create, edit, delete customers (name, email, phone, address) |
| 2 | Account management | Open, edit, close savings/checking accounts |
| 3 | Deposit | Deposit funds with description |
| 4 | Withdraw | Enforces savings monthly withdrawal limit |
| 5 | Transfer | Atomic two-leg transfer with unique transfer ID; currency mismatch rejected |
| 6 | Interest | Apply interest to savings accounts at configured rate |
| 7 | Balance inquiry | Shows type-specific details (rate, overdraft, monthly counter) |
| 8 | Transaction history | Filter by type, amount range, and date range; timestamped output |
| 9 | Statement | Period summary with opening/closing balances and net change |
| 10 | Notifications | Observer pattern: EMAIL and SMS console alerts on every transaction |

## Class Structure

```
src/
├── Person.h / .cpp              — abstract base with name/email/phone
├── Customer.h / .cpp            — Person subclass, owns accounts
├── Account.h / .cpp             — abstract account base (balance, transactions, observers)
├── SavingsAccount.h / .cpp      — monthly withdrawal limit, interest
├── CheckingAccount.h / .cpp     — overdraft limit, monthly fee
├── Transaction.h / .cpp         — immutable transaction record
├── Transfer.h / .cpp            — atomic two-account transfer
├── TransactionFilter.h / .cpp   — filter by type, amount, date range
├── Statement.h / .cpp           — period statement report
├── TransactionObserver.h        — observer interface
├── EmailNotifier.h / .cpp       — email notification observer
├── SmsNotifier.h / .cpp         — SMS notification observer
└── main.cpp                     — console menu application
```

## Demo Data

On startup a demo customer **C0001** (Alice Demo) with accounts **A0001** (savings, USD, 3% rate, limit 3/month) and **A0002** (checking, USD, $200 overdraft) is pre-loaded.
