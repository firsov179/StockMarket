#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>


#include "json.hpp"
#include "Common.hpp"
#include "Core.h"

using boost::asio::ip::tcp;

Core &GetCore() {
    static Core core;
    return core;
}

class session {
public:
    session(boost::asio::io_service &io_service)
            : socket_(io_service) {
    }

    tcp::socket &socket() {
        return socket_;
    }

    void start() {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code &error,
                     size_t bytes_transferred) {
        if (!error) {
            data_[bytes_transferred] = '\0';

            // Парсим json, который пришёл нам в сообщении.
            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];
            unsigned long UserId = std::stol(static_cast<const std::string &>(j["UserId"]));

            std::string reply = "Error! Unknown request typen";
            std::cout << reqType << std::endl;
            if (reqType == Requests::Registration) {
                // Это реквест на регистрацию пользователя.
                // Добавляем нового пользователя и возвращаем его ID.
                long passwordHash = std::stol(static_cast<const std::string &>(j["PasswordHash"]));
                reply = std::to_string(GetCore().RegisterNewUser(j["Name"], passwordHash));
            } else if (reqType == Requests::Login) {
                long passwordHash = std::stol(static_cast<const std::string &>(j["PasswordHash"]));
                reply = std::to_string(GetCore().LoginUser(j["Name"], passwordHash));
            } else if (!GetCore().checkUserId(UserId)) {
                // Проверка корректного userId
                reply = "Error! Bad userId in request\n";
            } else if (reqType == Requests::Hello) {
                // Это реквест на приветствие.
                // Находим имя пользователя по ID и приветствуем его по имени.
                reply = "Hello, " + GetCore().GetUserName(UserId) + "!\n";
            } else if (reqType == Requests::Balance) {
                // Это реквест на вывод баланса.
                // Находим имя пользователя по ID и выводим его баланс.
                reply = GetCore().GetUserBalance(UserId).toString();
            }else if (reqType == Requests::Quotes) {
                reply = "";
                for (auto item : GetCore().quotes) {
                    reply += " " + std::to_string(item);
                }
                reply += "\n";
            } else if (reqType == Requests::AddOrder) {
                unsigned int quantity = std::stoi(static_cast<const std::string &>(j["Quantity"]));
                int cost = std::stoi(static_cast<const std::string &>(j["Cost"]));
                Order curOrder(UserId, quantity, cost);
                if (j["OrderType"] == "Sell") {
                    GetCore().AddSell(curOrder);
                } else {
                    GetCore().AddPurchase(curOrder);
                }
                reply = "Ok!\n";
            }

            boost::asio::async_write(socket_,
                                     boost::asio::buffer(reply, reply.size()),
                                     boost::bind(&session::handle_write, this,
                                                 boost::asio::placeholders::error));
        } else {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code &error) {
        if (!error) {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        } else {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum {
        max_length = 1024
    };
    char data_[max_length];
};

class server {
public:
    server(boost::asio::io_service &io_service)
            : io_service_(io_service),
              acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        session *new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void handle_accept(session *new_session,
                       const boost::system::error_code &error) {
        if (!error) {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                                   boost::bind(&server::handle_accept, this, new_session,
                                               boost::asio::placeholders::error));
        } else {
            delete new_session;
        }
    }

private:
    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
};

int main() {
    try {
        boost::asio::io_service io_service;
        static Core core;

        server s(io_service);

        io_service.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}