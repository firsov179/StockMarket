#pragma once

#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <gtest/gtest.h>

#include "Balance.h"
#include "Order.h"


class Core {
public:

    Core() {
        orders = {};
    }
    // "Регистрирует" нового пользователя и возвращает его ID.
    unsigned long RegisterNewUser(const std::string &aUserName, long aUserPasswordHash);

    unsigned long LoginUser(const std::string &aUserName, long aUserPasswordHash);

    // Запрос имени клиента по ID
    std::string GetUserName(unsigned long aUserId);

    bool checkUserId(unsigned long aUserId);

    Balance GetUserBalance(unsigned long aUserId);

    void AddSell(size_t sellOrder);

    void AddPurchase(size_t purchOrder);

    std::vector<int> quotes;
    std::unordered_map<unsigned long, std::vector<std::string>> alerts;

    std::string GetUserList(unsigned long i);

    size_t CreateOrder(unsigned long userId, unsigned int quantity, int cost, bool isSale);
    static std::vector<Order> orders;

    std::string Cansel(unsigned long userId, int index);

private:
    struct sellCmp {
        bool operator()(const size_t a, const size_t b) const;
    };

    struct purchCmp {
        bool operator()(const size_t a, const size_t b) const;
    };

    // <userId, <UserName, password hash>>
    std::unordered_map<unsigned long, std::pair<std::string, long>> mUsers;
    // <UserName, userId>
    std::unordered_map<std::string, unsigned long> allUsers;
    std::unordered_map<unsigned long, std::vector<size_t>> usersOrders;

    // <userId, <USD, RUB>>
    std::unordered_map<unsigned long, Balance> mUserBalance;

    void transaction(unsigned long sellerId, unsigned long buyerId,
                       int curCost, unsigned int curQuantity);

    std::set<size_t, sellCmp> mSalesOrder;
    std::set<size_t, purchCmp> mPurchasesOrder;

    FRIEND_TEST(AddPurchase, SimpleAddition);
    FRIEND_TEST(AddPurchase, Transaction);
    FRIEND_TEST(AddPurchase, Cansel);
};
