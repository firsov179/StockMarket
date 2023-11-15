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
    /*!
     * Закрытие панели регистрации/входа.
     */
    void on_close_clicked();

    /*!
     * Открытие панели входа по логину/паролю.
     */
    void on_Login_clicked();

    /*!
     *  Открытие панели входа по логину/паролю.
     */
    void on_Registration_clicked();

    /*!
     * Регистрация нового пользователя или вход по логину/паролю.
     */
    void on_ok_clicked();

    /*!
     * Закрытие приложения.
     */
    void on_exit_clicked();

    /*!
     * Обновление текущего списка.
     */
    void on_UpdateList_clicked();

    /*!
     * Создание новой заявки.
     */
    void on_AddOrder_clicked();

    /*!
     * Отмена текущей заявки.
     */
    void on_CanselOrder_clicked();

    /*!
     * Отмена текущей заявки.
     */
    void on_UpdateQuotes_clicked();

    /*!
     * Обновление баланса.
     */
    void on_UpdateBalance_clicked();

    /*!
     * Прослушивание входящих уведомлений от сервера.
     */
    void alertsListener();

    /*!
     * Переключение на список закрытых сделок.
     */
    void on_closed_clicked();

    /*!
     * Переключение на список актуальных заявок.
     */
    void on_actual_clicked();

private:
    Ui::StockMarket *ui;
    QTimer *timer;

};
#endif // STOCKMARKET_H
