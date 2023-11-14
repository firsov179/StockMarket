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
    unsigned long RegisterNewUser(const std::string &aUserName, long &aUserPasswordHash);

    unsigned long LoginUser(const std::string &aUserName, long aUserPasswordHash);

    // Запрос имени клиента по ID
    std::string GetUserName(unsigned long aUserId);

    bool checkUserId(unsigned long aUserId);

    Balance GetUserBalance(unsigned long aUserId);

    void AddSell(Order sellOrder);

    void AddPurchase(Order purchOrder);

    std::vector<int> quotes;
private:
    // <UserId, <UserName, password hash>>
    std::unordered_map<unsigned long, std::pair<std::string, long>> mUsers;
    // <UserName, UserId>
    std::unordered_map<std::string, unsigned long> allUsers;

    // <UserId, <USD, RUB>>
    std::map<unsigned long, Balance> mUserBalance;

    void transaction(unsigned long sellerId, unsigned long buyerId,
                       int curCost, unsigned int curQuantity);

    std::set<Order, sellCmp> mSalesOrder;
    std::set<Order, purchCmp> mPurchasesOrder;

    FRIEND_TEST(AddPurchase, simple);
    FRIEND_TEST(AddPurchase, Transaction);


};
