#include <gtest/gtest.h>

#include "Core.h"

TEST(AddPurchase, SimpleAddition) {
    static Core core;
    std::vector userId = {core.RegisterNewUser("Alice", 101),
                          core.RegisterNewUser("Bob", 102),
                          core.RegisterNewUser("Sam", 103)};
    std::vector cost = {80, 70, 75, 82, 30};
    for (int i = 0; i < 5; ++i) {
        auto index = core.CreateOrder(userId[i % 3], 10 * (i + 1), cost[i], false);
        core.AddPurchase(index);
    }
    ASSERT_EQ(core.mSalesOrder.size(), 0);
    ASSERT_EQ(core.mPurchasesOrder.size(), 5);
    cost = {82, 80, 75, 70, 30};
    int i = 0;
    for (auto &item: core.mPurchasesOrder) {
        ASSERT_EQ(cost[i], core.orders[item].cost);
        i++;
    }
    ASSERT_EQ(core.orders[*core.mPurchasesOrder.begin()].cost, 82);
}

TEST(AddPurchase, Transaction) {
    static Core core;
    std::vector userId = {core.RegisterNewUser("Alice", 101),
                          core.RegisterNewUser("Bob", 102),
                          core.RegisterNewUser("Sam", 103)};

    auto index = core.CreateOrder(userId[0], 10, 62, false);
    core.AddPurchase(index);
    index = core.CreateOrder(userId[1], 20, 63, false);
    core.AddPurchase(index);
    index = core.CreateOrder(userId[2], 50, 61, true);
    core.AddSell(index);

    ASSERT_EQ(core.mPurchasesOrder.size(), 0);
    ASSERT_EQ(core.mSalesOrder.size(), 1);
    ASSERT_EQ(core.orders[*core.mSalesOrder.begin()].quantity, 20);

    ASSERT_EQ(core.mUserBalance[userId[0]].usd, 10);
    ASSERT_EQ(core.mUserBalance[userId[1]].usd, 20);
    ASSERT_EQ(core.mUserBalance[userId[2]].usd, -30);

    ASSERT_EQ(core.mUserBalance[userId[0]].rub, -620);
    ASSERT_EQ(core.mUserBalance[userId[1]].rub, -1260);
    ASSERT_EQ(core.mUserBalance[userId[2]].rub, 1880);
}

TEST(AddPurchase, Balance) {
    static Core core;
    std::vector userId = {core.RegisterNewUser("Alice", 101),
                          core.RegisterNewUser("Bob", 102),
                          core.RegisterNewUser("Sam", 103)};

    auto index = core.CreateOrder(userId[0], 10, 62, false);
    core.AddPurchase(index);
    index = core.CreateOrder(userId[1], 20, 63, false);
    core.AddPurchase(index);
    index = core.CreateOrder(userId[2], 50, 61, true);
    core.AddSell(index);

    auto res =core.GetUserBalance(userId[0]);
    ASSERT_EQ(res.toString(), "Your balance: -620 RUB, 10 USD.\n");
    res = core.GetUserBalance(userId[1]);
    ASSERT_EQ(res.toString(), "Your balance: -1260 RUB, 20 USD.\n");
    res = core.GetUserBalance(userId[2]);
    ASSERT_EQ(res.toString(), "Your balance: 1880 RUB, -30 USD.\n");
    auto quotes = core.GetQuotes();
    ASSERT_EQ(quotes, "63 62\n");
}

TEST(AddPurchase, Cansel) {
    static Core core;
    auto userId = core.RegisterNewUser("Alice", 101);

    auto index1 = core.CreateOrder(userId, 1, 10, false);
    core.AddPurchase(index1);
    auto index2 = core.CreateOrder(userId, 1, 100, true);
    core.AddSell(index2);

    auto res = core.GetActualList(userId);

    std::string rhs = (boost::format("1. Purchase: Quantity: 1, Cost: 10, Creation time: %1%2. Sell: Quantity: 1, Cost: 100, Creation time: %2%")
            % std::asctime(std::localtime(&core.orders[index1].creationTime))
            % std::asctime(std::localtime(&core.orders[index1].creationTime))).str();
    ASSERT_EQ(res, rhs);

    res = core.Cansel(userId, 1);
    ASSERT_EQ(res, "Ok!\n");
    res = core.GetActualList(userId);
    rhs = (boost::format("1. Sell: Quantity: 1, Cost: 100, Creation time: %1%")
           % std::asctime(std::localtime(&core.orders[index1].creationTime))).str();
    ASSERT_EQ(res, rhs);

}

std::vector<Order> Core::orders = {};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}