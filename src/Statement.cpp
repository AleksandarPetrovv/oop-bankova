#include "Statement.h"

#include "Account.h"

#include <iomanip>
#include <iostream>

Statement::Statement(Account* account,
                     std::time_t periodStart,
                     std::time_t periodEnd,
                     double openingBalance,
                     double closingBalance,
                     std::vector<Transaction> transactions)
    : account(account),
      periodStart(periodStart),
      periodEnd(periodEnd),
      openingBalance(openingBalance),
      closingBalance(closingBalance),
      transactions(std::move(transactions)) {}

static std::string formatTime(std::time_t t) {
    char buf[32];
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

void Statement::generate() const {
    std::cout << "\n================ STATEMENT ================\n";
    if (account) {
        std::cout << "Account : " << account->getAccountId()
                  << " (" << account->getType() << ", " << account->getCurrency() << ")\n";
    }
    std::cout << "Period  : " << formatTime(periodStart)
              << "  ->  " << formatTime(periodEnd) << "\n";
    std::cout << "Opening : " << std::fixed << std::setprecision(2) << openingBalance << "\n";
    std::cout << "Closing : " << std::fixed << std::setprecision(2) << closingBalance << "\n";
    std::cout << "-------------------------------------------\n";
    for (const auto& tx : transactions) {
        std::cout << formatTime(tx.getDate()) << "  "
                  << std::setw(12) << std::left << Transaction::typeToString(tx.getType())
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2) << tx.getAmount()
                  << "  " << tx.getDescription() << "\n";
    }
    std::cout << "-------------------------------------------\n";
    std::cout << "Deposits total   : " << getDepositsTotal() << "\n";
    std::cout << "Withdrawals total: " << getWithdrawalsTotal() << "\n";
    std::cout << "Transfers count  : " << getTransfersCount() << "\n";
    std::cout << "===========================================\n";
}

double Statement::getDepositsTotal() const {
    double total = 0;
    for (const auto& tx : transactions) {
        if (tx.getType() == TransactionType::DEPOSIT) total += tx.getAmount();
    }
    return total;
}

double Statement::getWithdrawalsTotal() const {
    double total = 0;
    for (const auto& tx : transactions) {
        if (tx.getType() == TransactionType::WITHDRAWAL) total += tx.getAmount();
    }
    return total;
}

int Statement::getTransfersCount() const {
    int count = 0;
    for (const auto& tx : transactions) {
        if (tx.getType() == TransactionType::TRANSFER_IN ||
            tx.getType() == TransactionType::TRANSFER_OUT) {
            ++count;
        }
    }
    return count;
}
