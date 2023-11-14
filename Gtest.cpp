#include <gtest/gtest.h>


#include "Core.h"

TEST(AddPurchase, simple) {
    static Core core;
    std::vector userId = {core.RegisterNewUser("Alice", 101),
                   core.RegisterNewUser("Bob", 102),
                   core.RegisterNewUser("Sam", 103)};
    std::vector cost = {80, 70, 75, 82, 30};
    for (int i = 0; i < 5; ++i) {
        auto index = core.CreateOrder(userId[i % 3], 10 * (i + 1), cost[i]);
        core.AddPurchase(index);
    }
    ASSERT_EQ(core.mSalesOrder.size(), 0);
    ASSERT_EQ(core.mPurchasesOrder.size(), 5);
    cost = {82, 80, 75, 70, 30};
    int i = 0;
    for (auto& item : core.mPurchasesOrder) {
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

    auto index = core.CreateOrder(userId[0], 10 , 62);
    core.AddPurchase(index);
    index = core.CreateOrder(userId[1], 20 , 63);
    core.AddPurchase(index);
    index = core.CreateOrder(userId[2], 50 , 61);
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

std::vector<Order> Core::orders = {};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}