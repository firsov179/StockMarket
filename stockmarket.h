#ifndef STOCKMARKET_H
#define STOCKMARKET_H

#include <thread>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class StockMarket; }
QT_END_NAMESPACE

class StockMarket : public QMainWindow
{
    Q_OBJECT

public:
    StockMarket(QWidget *parent = nullptr);
    ~StockMarket();

private slots:
    void on_Registration_clicked();

    void on_close_clicked();

    void on_Login_clicked();

    void on_ok_clicked();

    void on_exit_clicked();

    void on_UpdateList_clicked();

    void on_AddOrder_clicked();

    void on_CanselOrder_clicked();

    void on_UpdateQuotes_clicked();

    void on_UpdateBalance_clicked();

    void alertsListener();

    void on_closed_clicked();

    void on_actual_clicked();

private:
    Ui::StockMarket *ui;
    QTimer *timer;

};
#endif // STOCKMARKET_H
