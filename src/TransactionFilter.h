#pragma once

#include "Transaction.h"

#include <ctime>
#include <optional>

class TransactionFilter {
public:
    std::optional<std::time_t> startDate;
    std::optional<std::time_t> endDate;
    std::optional<TransactionType> type;
    std::optional<double> minAmount;
    std::optional<double> maxAmount;

    bool matches(const Transaction& tx) const;
};
