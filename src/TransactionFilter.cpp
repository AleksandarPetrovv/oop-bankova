#include "TransactionFilter.h"

bool TransactionFilter::matches(const Transaction& tx) const {
    if (startDate && tx.getDate() < *startDate) return false;
    if (endDate   && tx.getDate() > *endDate)   return false;
    if (type      && tx.getType() != *type)     return false;
    if (minAmount && tx.getAmount() < *minAmount) return false;
    if (maxAmount && tx.getAmount() > *maxAmount) return false;
    return true;
}
