#include <memory>
#include "Core.h"

unsigned long Core::RegisterNewUser(const std::string &aUserName, long aUserPasswordHash) {
    if (allUsers.find(aUserName) != allUsers.end()) {
        return 0;
    }
    unsigned long newUserId = mUsers.size() + 1;
    allUsers[aUserName] = newUserId;
    alerts[newUserId] = {};
    mUsers[newUserId] = {aUserName, aUserPasswordHash};
    mUserBalance[newUserId] = Balance();
    return newUserId;
}

unsigned long Core::LoginUser(const std::string &aUserName, long aUserPasswordHash) {
    auto it = allUsers.find(aUserName);
    if (it == allUsers.end()) {
        return 0;
    }
    if (mUsers[it->second].second != aUserPasswordHash) {
        return 0;
    }
    return it->second;
}

std::string Core::GetUserName(const unsigned long aUserId) {
    const auto &userIt = mUsers.find(aUserId);
    return userIt->second.first;
}

Balance Core::GetUserBalance(const unsigned long aUserId) {
    const auto userIt = mUserBalance.find(aUserId);
    return userIt->second;
}

bool Core::checkUserId(unsigned long aUserId) {
    const auto userIt = mUsers.find(aUserId);
    if (userIt == mUsers.cend()) {
        return false;
    } else {
        return true;
    }
}

void Core::AddSell(size_t sellOrder) {
    if (mPurchasesOrder.empty() || orders[sellOrder].cost > orders[*mPurchasesOrder.begin()].cost) {
        mSalesOrder.insert(sellOrder);
        return;
    }
    while (!mPurchasesOrder.empty() && orders[sellOrder].quantity != 0 &&
           orders[*mPurchasesOrder.begin()].cost >= orders[sellOrder].cost) {
        if (orders[sellOrder].quantity < orders[*mPurchasesOrder.begin()].quantity) {
            Order updated(orders[*mPurchasesOrder.begin()]);
            updated.quantity -= orders[sellOrder].quantity;

            transaction(orders[sellOrder].userId, updated.userId, updated.cost,
                        orders[sellOrder].quantity);

            orders[sellOrder].quantity = 0;
            orders.push_back(updated);
            mSalesOrder.erase(mPurchasesOrder.begin());
            mPurchasesOrder.insert(orders.size() - 1);

        } else {
            transaction(orders[sellOrder].userId, orders[*mPurchasesOrder.begin()].userId,
                        orders[*mPurchasesOrder.begin()].cost, orders[*mPurchasesOrder.begin()].quantity);

            orders[sellOrder].quantity -= orders[*mPurchasesOrder.begin()].quantity;
            mPurchasesOrder.erase(mPurchasesOrder.begin());
        }
    }
    if (orders[sellOrder].quantity != 0) {
        mSalesOrder.insert(sellOrder);
    }
}

void Core::AddPurchase(size_t purchOrder) {
    if (mSalesOrder.empty() || orders[purchOrder].cost < orders[*mSalesOrder.begin()].cost) {
        mPurchasesOrder.insert(purchOrder);
        return;
    }
    while (!mPurchasesOrder.empty() && orders[purchOrder].quantity != 0 &&
           orders[*mSalesOrder.begin()].cost <= orders[purchOrder].cost) {
        if (orders[purchOrder].quantity < orders[*mSalesOrder.begin()].quantity) {
            Order updated(orders[*mSalesOrder.begin()]);
            updated.quantity -= orders[purchOrder].quantity;

            transaction(updated.userId, orders[purchOrder].userId, orders[purchOrder].cost,
                        orders[purchOrder].quantity);

            orders[purchOrder].quantity = 0;
            orders.push_back(updated);
            mSalesOrder.erase(mSalesOrder.begin());
            mSalesOrder.insert(orders.size() - 1);
        } else {
            transaction(orders[*mSalesOrder.begin()].userId, orders[purchOrder].userId,
                        orders[purchOrder].cost, orders[*mSalesOrder.begin()].quantity);

            orders[purchOrder].quantity -= orders[*mSalesOrder.begin()].quantity;
            mSalesOrder.erase(mSalesOrder.begin());
        }
    }
    if (orders[purchOrder].quantity != 0) {
        mPurchasesOrder.insert(purchOrder);
    }
}

bool Core::sellCmp::operator()(const size_t a, const size_t b) const {
    if (orders[a].cost != orders[b].cost) {
        return orders[a].cost < orders[b].cost;
    }
    return orders[a].creationTime < orders[b].creationTime;
}


bool Core::purchCmp::operator()(const size_t a, const size_t b) const {
    if (orders[a].cost != orders[b].cost) {
        return orders[a].cost > orders[b].cost;
    }
    return orders[a].creationTime < orders[b].creationTime;
}

void Core::transaction(unsigned long sellerId, unsigned long buyerId, int curCost, unsigned int curQuantity) {
    long deltaRub = curQuantity * curCost;
    mUserBalance[sellerId].rub += deltaRub;
    mUserBalance[sellerId].usd -= curQuantity;
    mUserBalance[buyerId].rub -= deltaRub;
    mUserBalance[buyerId].usd += curQuantity;
    quotes.push_back(curCost);
    alerts[sellerId].push_back((boost::format(
            "Your application for the sale of %1% USD is executed at a price of %2% rubles per dollar.\n") %
                                curQuantity % curCost).str());
    alerts[buyerId].push_back((boost::format(
            "Your application for the purchase of %1% USD is executed at a price of %2% rubles per dollar.\n") %
                               curQuantity % curCost).str());
}

std::string Core::GetUserList(unsigned long i) {
    return "";
}

size_t Core::CreateOrder(unsigned long UserId, unsigned int quantity, int cost) {
    Order curOrder(UserId, quantity, cost);
    size_t orderIndex = orders.size();
    orders.push_back(curOrder);
    return orderIndex;
}

