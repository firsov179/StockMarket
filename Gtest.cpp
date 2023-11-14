#include <gtest/gtest.h>


#include "Core.h"

TEST(AddPurchase, simple) {
    static Core core;
    std::vector userId = {core.RegisterNewUser("Alice"),
                   core.RegisterNewUser("Bob"),
                   core.RegisterNewUser("Sam")};
    std::vector cost = {80, 70, 75, 82, 30};
    for (int i = 0; i < 5; ++i) {
        Order curOrder(userId[i % 3], 10 * (i + 1), cost[i]);
        core.AddPurchase(curOrder);
    }
    ASSERT_EQ(core.mSalesOrder.size(), 0);
    ASSERT_EQ(core.mPurchasesOrder.size(), 5);
    cost = {82, 80, 75, 70, 30};
    int i = 0;
    for (auto& item : core.mPurchasesOrder) {
        ASSERT_EQ(cost[i], item.cost);
        i++;
    }
    ASSERT_EQ(core.mPurchasesOrder.begin()->cost, 82);
}

TEST(AddPurchase, Transaction) {
    static Core core;
    std::vector userId = {core.RegisterNewUser("Alice"),
                          core.RegisterNewUser("Bob"),
                          core.RegisterNewUser("Sam")};

    Order aliceOrder(userId[0], 10 , 62);
    Order bobOrder(userId[1], 20 , 63);
    Order samOrder(userId[2], 50 , 61);

    core.AddPurchase(aliceOrder);
    core.AddPurchase(bobOrder);
    core.AddSell(samOrder);

    ASSERT_EQ(core.mPurchasesOrder.size(), 0);
    ASSERT_EQ(core.mSalesOrder.size(), 1);
    ASSERT_EQ(core.mSalesOrder.begin()->quantity, 20);

    ASSERT_EQ(core.mUserBalance[userId[0]].usd, 10);
    ASSERT_EQ(core.mUserBalance[userId[1]].usd, 20);
    ASSERT_EQ(core.mUserBalance[userId[2]].usd, -30);

    ASSERT_EQ(core.mUserBalance[userId[0]].rub, -620);
    ASSERT_EQ(core.mUserBalance[userId[1]].rub, -1260);
    ASSERT_EQ(core.mUserBalance[userId[2]].rub, 1880);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}