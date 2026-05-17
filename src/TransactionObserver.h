#pragma once

class Transaction;

class TransactionObserver {
public:
    virtual ~TransactionObserver() = default;
    virtual void onTransaction(const Transaction& tx) = 0;
};
