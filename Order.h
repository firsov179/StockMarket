#pragma once

#include <chrono>
#include <ctime>

struct Order {
    unsigned long userId;
    unsigned int quantity;
    int cost;
    std::time_t creationTime;

    Order(unsigned long userId_, unsigned int quantity_, int cost_) {
        userId = userId_;
        quantity = quantity_;
        cost = cost_;
        creationTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    Order(const Order &other) {
        userId = other.userId;
        quantity = other.quantity;
        cost = other.cost;
        creationTime = other.creationTime;
    }
};
