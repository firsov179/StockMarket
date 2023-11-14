#pragma once

#include <chrono>
#include <ctime>
#include <string>

struct Order {
    unsigned long userId;
    unsigned int quantity;
    int cost;
    std::time_t creationTime;
    bool isSell;

    Order(unsigned long userId_, unsigned int quantity_, int cost_, bool isSell_) {
        userId = userId_;
        quantity = quantity_;
        cost = cost_;
        creationTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        isSell = isSell_;
    }

    std::string toString() {
        std::string res = isSell ? "Sell: " : "Purchase: ";
        return res + (boost::format("Quantity: %1%, Cost: %2%, Creation time: %3%") % quantity % cost % std::asctime(std::localtime(&creationTime))).str();
    }
};
