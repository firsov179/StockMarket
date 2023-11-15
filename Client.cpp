#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <thread>

#include "Common.hpp"
#include "ClientUnion.h"

using boost::asio::ip::tcp;
std::string Global::my_id = "0";
std::mutex Global::mutex;

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket &aSocket, std::string type) {
    std::string name, password;
    std::cout << "Enter your name: ";
    std::cin >> name;
    std::cout << "Enter your password: ";
    std::cin >> password;
    return Global::ProcessRegistration(aSocket, type, name, password);
}

void alertsListener(std::string my_id) {
    boost::asio::io_service io_service_listener;
    tcp::resolver resolver_listener(io_service_listener);
    tcp::resolver::query query_listener(tcp::v4(), "127.0.0.1", std::to_string(port));
    tcp::resolver::iterator iterator_listener = resolver_listener.resolve(query_listener);

    tcp::socket s_listener(io_service_listener);
    s_listener.connect(*iterator_listener);
    while (true) {
        {
            const std::lock_guard<std::mutex> lock(Global::mutex);
            Global::SendMessage(s_listener, my_id, Requests::Check);
            auto x =  Global::ReadMessage(s_listener);
            if (x != "Bye!\n") {
                std::cout << x;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


int main() {
    try {
        boost::asio::io_service io_service, io_service_listener;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);
        std::string my_id;
        std::cout << "Loggin Menu:\n"
                     "1) Registration\n"
                     "2) Login\n"
                     "3) Exit\n"
                  << std::endl;

        short menuIndex;
        std::cin >> menuIndex;
        switch (menuIndex) {
            case 1: {
                my_id = ProcessRegistration(s, Requests::Registration);
                while (my_id == "0") {
                    std::cout << "This name has already been chosen by someone. Please, choose something else.\n";
                    my_id = ProcessRegistration(s, Requests::Registration);
                }
                break;
            }
            case 2: {
                my_id = ProcessRegistration(s, Requests::Login);
                while (my_id == "0") {
                    std::cout << "Invalid username or password.\n";
                    my_id = ProcessRegistration(s, Requests::Login);
                }
                break;
            }
            case 3: {
                exit(0);
                break;
            }
            default: {
                std::cout << "Unknown menu option\n" << std::endl;
            }
        }
        std::thread thread(alertsListener, my_id);
        while (true) {
            // Тут реализовано "бесконечное" меню.
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
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::Hello);
                        std::cout << Global::ReadMessage(s);
                    }
                    break;
                }
                case 2: {
                    // Этот метод получает от сервера информацию о балансе клиента,
                    // отправляя серверу id, полученный при регистрации.
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::Balance);
                        std::cout << Global::ReadMessage(s);
                    }
                    break;
                }
                case 3: {
                    unsigned int quantity;
                    int cost;
                    std::cout << "Enter the cost: ";
                    std::cin >> cost;
                    std::cout << "Enter the quantity: ";
                    std::cin >> quantity;
                    std::cout << "Do you want sell the USD: (Enter Y or y to sell, other to Buy)";
                    char buffer;
                    std::cin >> buffer;
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::AddOrder, {
                                {"Quantity",  std::to_string(quantity)},
                                {"Cost",      std::to_string(cost)},
                                {"OrderType", ((buffer == 'y' || buffer == 'Y') ? "Sell" : "Buy")}
                        });
                        std::cout << Global::ReadMessage(s);
                    }
                    break;
                }
                case 4: {
                    std::cout << "Your actual orders:\n";
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::ListActual);
                        std::cout << Global::ReadMessage(s);
                    }
                    break;
                }
                case 5: {
                    std::cout << "Your closed orders:\n";
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::ListClosed);
                        std::cout << Global::ReadMessage(s);
                    }
                    break;
                }
                case 6: {
                    std::cout << "Your current orders:\n";
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::ListActual);
                        std::cout << Global::ReadMessage(s);
                    }
                    int n;
                    std::cout << "Enter the index of order, that you want to cansel.\n";
                    std::cin >> n;
                    {
                        const std::lock_guard<std::mutex> lock(Global::mutex);
                        Global::SendMessage(s, my_id, Requests::Cansel, {{"Index", std::to_string(n)}});
                        std::cout << Global::ReadMessage(s);
                    }
                    break;
                }
                case 7: {
                    std::cout << "Quotes from the beginning of trading: ";
                    Global::SendMessage(s, my_id, Requests::Quotes);
                    std::cout << Global::ReadMessage(s);
                    break;
                }
                case 8: {
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