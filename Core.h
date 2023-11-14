#pragma once

#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <gtest/gtest.h>

#include "Balance.h"
#include "Order.h"

struct sellCmp {
    bool operator()(const Order &a, const Order &b) const;
};

struct purchCmp {
    bool operator()(const Order &a, const Order &b) const;
};


class Core {
public:
    // "Регистрирует" нового пользователя и возвращает его ID.
    const unsigned long RegisterNewUser(const std::string &aUserName);

    // Запрос имени клиента по ID
    std::string GetUserName(unsigned long aUserId);

    bool checkUserId(unsigned long aUserId);

    Balance GetUserBalance(unsigned long aUserId);

    void AddSell(Order sellOrder);

    void AddPurchase(Order purchOrder);

    std::vector<int> quotes;
private:
    // <UserId, UserName>
    std::unordered_map<unsigned long, std::string> mUsers;
    // <UserId, <USD, RUB>>
    std::map<unsigned long, Balance> mUserBalance;

    void transaction(unsigned long sellerId, unsigned long buyerId,
                       int curCost, unsigned int curQuantity);

    std::set<Order, sellCmp> mSalesOrder;
    std::set<Order, purchCmp> mPurchasesOrder;

    FRIEND_TEST(AddPurchase, simple);
    FRIEND_TEST(AddPurchase, Transaction);


};
