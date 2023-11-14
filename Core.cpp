#include <memory>
#include "Core.h"


unsigned long Core::RegisterNewUser(const std::string &aUserName, long &aUserPasswordHash) {
    if (allUsers.find(aUserName) != allUsers.end()) {
        return 0;
    }
    unsigned long newUserId = mUsers.size() + 1;
    allUsers[aUserName] = newUserId;
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
    const auto& userIt = mUsers.find(aUserId);
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

void Core::AddSell(Order sellOrder) {
    if (mPurchasesOrder.empty() || sellOrder.cost > mPurchasesOrder.begin()->cost) {
        mSalesOrder.insert(sellOrder);
        return;
    }
    while (!mPurchasesOrder.empty() && sellOrder.quantity != 0 && mPurchasesOrder.begin()->cost >= sellOrder.cost) {
        if (sellOrder.quantity < mPurchasesOrder.begin()->quantity) {
            Order updated(*mPurchasesOrder.begin());
            updated.quantity -= sellOrder.quantity;

            transaction(sellOrder.userId, updated.userId, updated.cost,
                          sellOrder.quantity);

            sellOrder.quantity = 0;
            mPurchasesOrder.erase(mPurchasesOrder.begin());
            mPurchasesOrder.insert(updated);
        } else {
            transaction(sellOrder.userId, mPurchasesOrder.begin()->userId,
                          mPurchasesOrder.begin()->cost, mPurchasesOrder.begin()->quantity);

            sellOrder.quantity -= mPurchasesOrder.begin()->quantity;
            mPurchasesOrder.erase(mPurchasesOrder.begin());
        }
    }
    if (sellOrder.quantity != 0) {
        mSalesOrder.insert(sellOrder);
    }
}

void Core::AddPurchase(Order purchOrder) {
    if (mSalesOrder.empty() || purchOrder.cost < mSalesOrder.begin()->cost) {
        mPurchasesOrder.insert(purchOrder);
        return;
    }
    while (!mPurchasesOrder.empty() && purchOrder.quantity != 0 && mSalesOrder.begin()->cost <= purchOrder.cost) {
        if (purchOrder.quantity < mSalesOrder.begin()->quantity) {
            Order updated(*mSalesOrder.begin());
            updated.quantity -= purchOrder.quantity;

            transaction(updated.userId, purchOrder.userId, purchOrder.cost, purchOrder.quantity);

            purchOrder.quantity = 0;
            mSalesOrder.erase(mSalesOrder.begin());
            mSalesOrder.insert(updated);
        } else {
            transaction(mSalesOrder.begin()->userId, purchOrder.userId,
                          purchOrder.cost, mSalesOrder.begin()->quantity);

            purchOrder.quantity -= mSalesOrder.begin()->quantity;
            mSalesOrder.erase(mSalesOrder.begin());
        }
    }
    if (purchOrder.quantity != 0) {
        mPurchasesOrder.insert(purchOrder);
    }
}

bool sellCmp::operator()(const Order &a, const Order &b) const {
    if (a.cost != b.cost) {
        return a.cost < b.cost;
    }
    return a.creationTime < b.creationTime;
}


bool purchCmp::operator()(const Order &a, const Order &b) const {
    if (a.cost != b.cost) {
        return a.cost > b.cost;
    }
    return a.creationTime < b.creationTime;
}

void Core::transaction(unsigned long sellerId, unsigned long buyerId, int curCost, unsigned int curQuantity) {
    long deltaRub = curQuantity * curCost;
    mUserBalance[sellerId].rub += deltaRub;
    mUserBalance[sellerId].usd -= curQuantity;
    mUserBalance[buyerId].rub -= deltaRub;
    mUserBalance[buyerId].usd += curQuantity;
    quotes.push_back(curCost);
}

