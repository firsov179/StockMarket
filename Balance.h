#pragma once

#include <boost/format.hpp>

struct Balance {
    long rub, usd;

    Balance() {
        rub = 0;
        usd = 0;
    }

    std::string toString() {
        return (boost::format("Balance: %1% RUB, %2% USD.\n") % rub % usd).str();
    }
};
