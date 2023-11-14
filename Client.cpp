#include <iostream>
#include <map>

#include <boost/asio.hpp>
#include <thread>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;
std::mutex mutex;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
        tcp::socket &aSocket,
        const std::string &aId,
        const std::string &aRequestType,
        std::map<std::string, std::string> aMessage = {}) {
    nlohmann::json req;
    req["userId"] = aId;
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
    {
        const std::lock_guard<std::mutex> lock(mutex);
        SendMessage(aSocket, "0", type, {{"Name",         name},
                                         {"PasswordHash", std::to_string(passwordHash)}});
        return ReadMessage(aSocket);
    }
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
            const std::lock_guard<std::mutex> lock(mutex);
            SendMessage(s_listener, my_id, Requests::Check);
            auto x =  ReadMessage(s_listener);
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
        //thread.join();
        while (true) {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Balance\n"
                         "3) Add Order\n"
                         "4) Cancel Order\n"
                         "5) Quotes\n"
                         "6) Exit\n"
                      << std::endl;
            
            std::cin >> menuIndex;
            switch (menuIndex) {
                case 1: {
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    {
                        const std::lock_guard<std::mutex> lock(mutex);
                        SendMessage(s, my_id, Requests::Hello);
                        std::cout << ReadMessage(s);
                    }
                    break;
                }
                case 2: {
                    // Этот метод получает от сервера информацию о балансе клиента,
                    // отправляя серверу id, полученный при регистрации.
                    {
                        const std::lock_guard<std::mutex> lock(mutex);
                        SendMessage(s, my_id, Requests::Balance);
                        std::cout << ReadMessage(s);
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
                        const std::lock_guard<std::mutex> lock(mutex);
                        SendMessage(s, my_id, Requests::AddOrder, {
                                {"Quantity",  std::to_string(quantity)},
                                {"Cost",      std::to_string(cost)},
                                {"OrderType", ((buffer == 'y' || buffer == 'Y') ? "Sell" : "Buy")}
                        });
                        std::cout << ReadMessage(s);
                    }
                    break;
                }
                case 4: {
                    std::cout << "Your current orders\n";
                    {
                        const std::lock_guard<std::mutex> lock(mutex);
                        SendMessage(s, my_id, Requests::List);
                        std::cout << ReadMessage(s);
                    }
                    int n;
                    std::cout << "Enter the index of order, that you want to cansel.\n";
                    std::cin >> n;
                    {
                        const std::lock_guard<std::mutex> lock(mutex);
                        SendMessage(s, my_id, Requests::Cansel, {{"Index", std::to_string(n)}});
                        std::cout << ReadMessage(s);
                    }
                    break;
                }
                case 5: {
                    std::cout << "Quotes from the beginning of trading: ";
                    SendMessage(s, my_id, Requests::Quotes);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 6: {
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