#pragma once

#include <boost/format.hpp>

struct Balance {
    // Почти std::pair, но с удобным названием полей
    long rub, usd;

    Balance() {
        rub = 0;
        usd = 0;
    }

    std::string toString() {
        // Представление в виде строки
        return (boost::format("Your balance: %1% RUB, %2% USD.\n") % rub % usd).str();
    }
};
