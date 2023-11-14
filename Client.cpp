#include <iostream>
#include <map>
#include <boost/asio.hpp>


#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
        tcp::socket &aSocket,
        const std::string &aId,
        const std::string &aRequestType,
        std::map<std::string, std::string> aMessage = {}) {
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    for (auto &item: aMessage) {
        req[item.first] = item.second;
    }

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket &aSocket) {
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket &aSocket, std::string type) {
    std::string name, password;
    std::cout << "Enter your name: ";
    std::cin >> name;
    std::cout << "Enter your password: ";
    std::cin >> password;
    long passwordHash = std::hash<std::string>{}(password);
    SendMessage(aSocket, "0", type, {{"Name",         name},
                                                       {"PasswordHash", std::to_string(passwordHash)}});
    return ReadMessage(aSocket);
}

int main() {
    try {
        boost::asio::io_service io_service;

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

        short menu_option_num;
        std::cin >> menu_option_num;
        switch (menu_option_num) {
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

        while (true) {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Balance\n"
                         "3) Add Order\n"
                         "4) Quotes\n"
                         "5) Exit\n"
                      << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num) {
                case 1: {
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Hello);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2: {
                    // Этот метод получает от сервера информацию о балансе клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Balance);
                    std::cout << ReadMessage(s);
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
                    SendMessage(s, my_id, Requests::AddOrder, {
                            {"Quantity",  std::to_string(quantity)},
                            {"Cost",      std::to_string(cost)},
                            {"OrderType", ((buffer == 'y' || buffer == 'Y') ? "Sell" : "Buy")}
                    });
                    std::cout << ReadMessage(s);
                    break;
                }
                case 4: {
                    std::cout << "Quotes from the beginning of trading: ";
                    SendMessage(s, my_id, Requests::Quotes);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 5: {
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