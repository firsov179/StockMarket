#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <thread>

#include "Common.hpp"
#include "ClientUnion.h"

using boost::asio::ip::tcp;


/*!
 * Запросы серверу о уведомлениях о совершенных сделках
 * @param my_id Идентификатор пользователя
 */
void alertsListener() {
    while (true) {
        {
            const std::lock_guard<std::mutex> lock(Global::mutex);
            Global::SendMessage( Requests::Check);
            auto x = Global::ReadMessage(Global::s);
            if (x != "Bye!\n") {
                std::cout << x;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

/*!
 * Запрос на сервер. Получение ответа.
 * @param reqType Тип запроса из Requests::.
 * @param aMessage Дополнительное сообщение в виде словаря.
 * @return Ответ сервера на запрос.
 */
std::string MakeRequest(std::string &reqType, std::map<std::string, std::string> aMessage = {}) {
    const std::lock_guard<std::mutex> lock(Global::mutex);
    Global::SendMessage( reqType, aMessage);
    return Global::ReadMessage(Global::s);
}


/*!
 * Регистрация пользователя по имени/логину и паролю.
 * Или вход уже созданного пользователя.
 * @param aSocket Сокет с сервером
 * @param type Requests::Registration или Requests::Login.
 * @return Ответ сервера - userId
 */
std::string ProcessRegistration(std::string type) {
    std::string name, password;
    std::cout << "Enter your name: ";
    std::cin >> name;
    std::cout << "Enter your password: ";
    std::cin >> password;
    long passwordHash = std::hash<std::string>{}(password);
    return MakeRequest(type, {
            {"Name",         name},
            {"PasswordHash", std::to_string(passwordHash)}});
}


boost::asio::io_service Global::io_service;
tcp::resolver::query Global::query(tcp::v4(), "127.0.0.1", std::to_string(port));
tcp::resolver Global::resolver(Global::io_service);
tcp::resolver::iterator Global::iterator = Global::resolver.resolve(Global::query);
tcp::socket Global::s(Global::io_service);

std::string Global::my_id = "0"; // инициализация userId
std::mutex Global::mutex; // Синхронизация потоков для доступа в сокет.


int main() {
    try {
        Global::s.connect(*Global::iterator);

        std::cout << "Loggin Menu:\n"
                     "1) Registration\n"
                     "2) Login\n"
                     "3) Exit\n"
                  << std::endl;

        short menuIndex;
        std::cin >> menuIndex;
        switch (menuIndex) {
            case 1: {
                // Регистрация нового пользователя.
                Global::my_id = ProcessRegistration(Requests::Registration);
                while (Global::my_id == "0") {
                    std::cout << "This name has already been chosen by someone. Please, choose something else.\n";
                    Global::my_id = ProcessRegistration(Requests::Registration);
                }
                break;
            }
            case 2: {
                // Вход по логину и паролю.
                Global::my_id = ProcessRegistration(Requests::Login);
                while (Global::my_id == "0") {
                    std::cout << "Invalid username or password.\n";
                    Global::my_id = ProcessRegistration(Requests::Login);
                }
                break;
            }
            case 3: {
                // Выход
                exit(0);
                break;
            }
            default: {
                std::cout << "Unknown menu option\n" << std::endl;
            }
        }
        std::thread thread(alertsListener);
        while (true) {
            std::cout << "Menu:\n"
                         "1) Hello request\n"
                         "2) Balance\n"
                         "3) Add order\n"
                         "4) List of actual orders\n"
                         "5) List of closed orders\n"
                         "6) Cancel order\n"
                         "7) Quotes\n"
                         "8) Exit\n"
                      << std::endl;

            std::cin >> menuIndex;
            switch (menuIndex) {
                case 1: {
                    // Этот метод получает от сервера приветствие с именем клиента.
                    std::cout << MakeRequest(Requests::Hello);
                    break;
                }
                case 2: {
                    // Этот метод получает от сервера информацию о балансе клиента.
                    std::cout << MakeRequest(Requests::Balance);
                    break;
                }
                case 3: {
                    // Создание новой заявки на покупку/продажу.
                    unsigned int quantity;
                    int cost;

                    std::cout << "Enter the cost: ";
                    std::cin >> cost;
                    std::cout << "Enter the quantity: ";
                    std::cin >> quantity;
                    std::cout << "Do you want sell the USD: (Enter Y or y to sell, other to Buy)";

                    char buffer;
                    std::cin >> buffer;
                    std::cout << MakeRequest(Requests::AddOrder, {
                            {"Quantity",  std::to_string(quantity)},
                            {"Cost",      std::to_string(cost)},
                            {"OrderType", ((buffer == 'y' || buffer == 'Y') ? "Sell" : "Buy")}
                    });
                    break;
                }
                case 4: {
                    // Получение списка всех актуальных заявок.
                    std::cout << "Your actual orders:\n";
                    std::cout << MakeRequest(Requests::ListActual);
                    break;
                }
                case 5: {
                    // Получение списка всех совершенных сделок.
                    std::cout << "Your closed orders:\n";
                    std::cout << MakeRequest(Requests::ListClosed);
                    break;
                }
                case 6: {
                    // Получение списка всех актуальных заявок.
                    std::cout << "Your current orders:\n";
                    std::cout << MakeRequest(Requests::ListActual);
                    int n;
                    // Отмена одной из заявок.
                    std::cout << "Enter the index of order, that you want to cansel.\n";
                    std::cin >> n;
                    std::cout << MakeRequest(Requests::Cansel, {{"Index", std::to_string(n)}});
                    break;
                }
                case 7: {
                    // Получение котировок - списка цен во всех совершенных сделках в порядке их совершения.
                    std::cout << "Quotes from the beginning of trading: ";
                    std::cout << MakeRequest(Requests::Quotes);
                    break;
                }
                case 8: {
                    // Выход
                    exit(0);
                    break;
                }
                default: {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}