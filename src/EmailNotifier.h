#pragma once

#include "TransactionObserver.h"

#include <string>

class EmailNotifier : public TransactionObserver {
private:
    std::string emailAddress;

public:
    explicit EmailNotifier(const std::string& emailAddress);
    void onTransaction(const Transaction& tx) override;
};
