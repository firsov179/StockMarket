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

    /*!
     * "Регистрирует" нового пользователя и возвращает его ID.
     * @param aUserName Имя пользователя
     * @param aUserPasswordHash Хэш пароля
     * @return userId. 0 если некорректные данные.
     */
    unsigned long RegisterNewUser(const std::string &aUserName, long aUserPasswordHash);

    /*!
     * Вход по логину паролю.
     * @param aUserName Имя пользователя.
     * @param aUserPasswordHash Хэш пароля.
     * @return userId. 0 если некорректные данные.
     */
    unsigned long LoginUser(const std::string &aUserName, long aUserPasswordHash);

    /*!
     * Запрос имени клиента по ID
     * @param aUserId userId пользователя
     * @return Имя пользователя
     */
    std::string GetUserName(unsigned long aUserId);

    /*!
     * Проверка корректности UserId
     * @param aUserId userId пользователя
     * @return существует ли пользователь с таким userId
     */
    bool checkUserId(unsigned long aUserId);

    /*!
     * Получение баланса пользователя.
     * @param aUserId userId пользователя
     * @return баланс пользователя.
     */
    Balance GetUserBalance(unsigned long aUserId);

    /*!
     * Добавление заявки на продажу.
     * @param sellOrder индекс заявки в orders.
     */
    void AddSell(size_t sellOrder);

    /*!
     * Добавление заявки на покупку.
     * @param sellOrder индекс заявки в orders.
     */
    void AddPurchase(size_t purchOrder);

    /*!
     * @param userId userId пользователя.
     * @return Список актуальных заявок на покупку и продажу.
     */
    std::string GetActualList(unsigned long userId);

    /*!
     * @param userId userId пользователя.
     * @return Список закрытых сделок на покупку и продажу.
     */
    std::string GetClosedList(unsigned long userId);

    /*!
     *
     * @param userId userId пользователя.
     * @param quantity количество USD выставленных на продажу/покупку.
     * @param cost цена одного USD.
     * @param isSale Является ли продажей.
     * @return индекс заявки в orders.
     */
    size_t CreateOrder(unsigned long userId, unsigned int quantity, int cost, bool isSale);

    /*!
     * Отмена заявки.
     * @param userId userId пользователя.
     * @param index индекс заявки в orders.
     * @return Сообщение пользователю.
     */
    std::string Cansel(unsigned long userId, int index);

    /*!
     * Получение данных котировок.
     * @return данные котировок.
     */
    std::string GetQuotes();

    // данные котировок
    std::vector<int> quotes;
    // данные о сообщениях пользователю
    std::unordered_map<unsigned long, std::vector<std::string>> alerts;
    // список всех заявок.
    static std::vector<Order> orders;


private:

    /*!
     * Совершение сделки между двумя заявками.
     * @param sellerId userId продавца.
     * @param buyerId userId покупателя.
     * @param curCost цена сделки.
     * @param curQuantity количество проданных USD.
     */
    void transaction(unsigned long sellerId, unsigned long buyerId,
                     int curCost, unsigned int curQuantity);

    // Компаратор для заявок на продажу.
    struct sellCmp {
        bool operator()(const size_t a, const size_t b) const;
    };

    // Компаратор для заявок на покупку.
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


    std::set<size_t, sellCmp> mSalesOrder;
    std::set<size_t, purchCmp> mPurchasesOrder;

    FRIEND_TEST(AddPurchase, SimpleAddition);

    FRIEND_TEST(AddPurchase, Transaction);

    FRIEND_TEST(AddPurchase, Cansel);

    FRIEND_TEST(AddPurchase, Balance);
};
