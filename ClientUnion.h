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

    /*!
     * Отправка сообщения на сервер по шаблону.
     * @param aRequestType Тип запроса из Requests::.
     * @param aMessage Дополнительные сообщения в виде словаря.
     */
    static void SendMessage(
        const std::string &aRequestType,
        std::map<std::string, std::string> aMessage = {}) {
        nlohmann::json req;
        req["userId"] = Global::my_id;
        req["ReqType"] = aRequestType;
        for (auto &item: aMessage) {
            req[item.first] = item.second;
        }

        std::string request = req.dump();
        boost::asio::write(Global::s, boost::asio::buffer(request, request.size()));
    }

    /*!
     * @param aSocket Сокет для общения с сервером
     * @return Возвращает строку с ответом сервера на последний запрос.
     */
    static std::string ReadMessage(tcp::socket &aSocket) {
        boost::asio::streambuf b;
        boost::asio::read_until(aSocket, b, "\0");
        std::istream is(&b);
        std::string line(std::istreambuf_iterator<char>(is), {});
        return line;
    }


    static boost::asio::io_service io_service;

    static tcp::resolver resolver;
    static tcp::resolver::query query;
    static tcp::resolver::iterator iterator;

    static tcp::socket s;
    static std::mutex mutex;
    static std::string my_id;
};




#endif
