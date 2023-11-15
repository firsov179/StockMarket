#pragma once

#include <chrono>
#include <ctime>
#include <string>

struct Order {
    // Вся информация об одной заявке.
    /*!
     * Конструктор заполняющий все поля.
     */
    Order(unsigned long userId_, unsigned int quantity_, int cost_, bool isSell_) {
        userId = userId_;
        quantity = quantity_;
        startQuantity = quantity_;
        cost = cost_;
        creationTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        isSell = isSell_;
    }

    /*!
     * @return Текстовая информация о заявке.
     */
    std::string toString() {
        std::string res = isSell ? "Sell: " : "Purchase: ";
        if (quantity == 0) {
            // Совершенная сделка
            res += "It's closed. ";
            return res + (boost::format("Quantity: %1%, Cost: %2%, Creation time: %3%") % startQuantity % cost % std::asctime(std::localtime(&creationTime))).str();
        }
        // Актуальная заявка
        res += "It's actual. ";
        return res + (boost::format("Quantity: %1%, Cost: %2%, Creation time: %3%") % quantity % cost % std::asctime(std::localtime(&creationTime))).str();
    }

    unsigned long userId;
    unsigned int quantity;
    unsigned int startQuantity;
    int cost;
    std::time_t creationTime;
    bool isSell;
};
