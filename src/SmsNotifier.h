#pragma once

#include "TransactionObserver.h"

#include <string>

class SmsNotifier : public TransactionObserver {
private:
    std::string phoneNumber;

public:
    explicit SmsNotifier(const std::string& phoneNumber);
    void onTransaction(const Transaction& tx) override;
};
