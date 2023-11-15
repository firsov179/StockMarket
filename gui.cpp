#include "stockmarket.h"
#include "ClientUnion.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

boost::asio::io_service Global::io_service;
tcp::resolver::query Global::query(tcp::v4(), "127.0.0.1", std::to_string(port));
tcp::resolver Global::resolver(Global::io_service);
tcp::resolver::iterator Global::iterator = Global::resolver.resolve(Global::query);
tcp::socket Global::s(Global::io_service);

std::string Global::my_id = "0";
std::mutex Global::mutex;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "nt_progress_gui_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    StockMarket w;
    Global::s.connect(*Global::iterator); // connect с сервером
    w.show();
    return a.exec();
}