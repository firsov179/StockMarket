# Тестовое задание для C++ разработчиков


## 📈 Легенда
---

Ты стал новым руководителем отдела биржи. Поздравляем! 🎉

Но выяснилось, что в твоём отделе уже несколько десятков лет торговля ведется по старинке! Все измучены миллионами звонков от трейдеров, выставляющих заявки по телефону. 🤳

Ты хочешь всё это автоматизировать, чтобы клиенты могли торговать через компьютер. Тогда все могли бы сидеть в тишине и обрабатывать их заявки ☺️

К счастью, у тебя есть для этого всё — Linux, C++ и полный гитхаб различных библиотек. 

## 🔮 Что необходимо сделать
---

Необходимо написать простую биржу с клиент-серверной архитектурой. Для упрощения задачи, биржа будет торговать только долларами (USD) за рубли (RUB).

Сервер должен принимать заявки на покупку или продажу долларов за рубли от разных клиентов по определённой цене. 

В случае, если цена на покупку и цена на продажу у нескольких клиентов пересекается — нужно заключать сделки между ними. В этом случае купленный объём будет зачисляться на баланс клиентам.

### Решение:
---

**Реализованы следующие возможности:**

- Сервер поддерживает подключения нескольких клиентов одновременно.
- Даёт возможность просмотра клиентом всех своих активных торговых заявок, всех своих совершенных сделок и своего баланса.
- Возможность просмотра клиентом котировок 📈
- Получение отчёта о совершённой сделке в момент её заключения всеми сторонами-участниками. 🤼
- Возможность отмены активных заявок клиентами. 🚫
- Авторизацию клиента с паролем. 🔑
- GUI к бирже на QT. 🤯


## 🧑‍💻 Реализация

Cmake собирает 3 исполняемых файла:
- Server 
- Client - консольный клиент.
- Gui - Графический интерфейс биржи.
- GTest - Запуск Unit тестов.

Для сборки проекта необходимы библиотеки:
- Boost (boost::asio, boost::bind, boost::format)
- Gtest 
- QT (Qt5 и Qt::Charts)

К основным функциям написана документация и Google тесты.
