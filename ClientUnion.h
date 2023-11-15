#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include <thread>

#include "Common.hpp"
#include "json.hpp"


using boost::asio::ip::tcp;

struct Global {

    // Отправка сообщения на сервер по шаблону.
    static void SendMessage(
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
    static std::string ReadMessage(tcp::socket &aSocket) {
        boost::asio::streambuf b;
        boost::asio::read_until(aSocket, b, "\0");
        std::istream is(&b);
        std::string line(std::istreambuf_iterator<char>(is), {});
        return line;
    }

    // "Создаём" пользователя, получаем его ID.
    static std::string ProcessRegistration(tcp::socket &aSocket, std::string name, std::string password, std::string type) {
        long passwordHash = std::hash<std::string>{}(password);
        {
            const std::lock_guard<std::mutex> lock(mutex);
            SendMessage(aSocket, "0", type, {{"Name",         name},
                                             {"PasswordHash", std::to_string(passwordHash)}});
            return ReadMessage(aSocket);
        }
    }


    static boost::asio::io_service io_service, io_service_listener;

    static tcp::resolver resolver;
    static tcp::resolver::query query;
    static tcp::resolver::iterator iterator;

    static tcp::socket s;
    static std::mutex mutex;
    static std::string my_id;
};




#endif
