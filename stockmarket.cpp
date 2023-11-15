#include "stockmarket.h"
#include "./ui_stockmarket.h"

#include "Common.hpp"
#include "ClientUnion.h"

#include <QMessageBox>
#include <QLineSeries>
#include <QChart>
#include <QTimer>
#include <QChartView>
#include <QGridLayout>


bool isRegistration = true;
QGridLayout* gridLayout;

StockMarket::StockMarket(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StockMarket) {
    ui->setupUi(this);
    ui->loginWidget->hide();
    ui->main->hide();
    ui->Quantity->setValidator( new QIntValidator(0, 1000, this));
    ui->Cost->setValidator( new QIntValidator(-1000, 1000, this));
    gridLayout = new QGridLayout(ui->Quotes);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(alertsListener()));
}

StockMarket::~StockMarket()
{
    delete ui;
}


void StockMarket::on_Registration_clicked()
{
    ui->ok->setText("Register");
    ui->heading->setText("Registration");
    ui->loginWidget->show();
    isRegistration = true;
}

void StockMarket::on_Login_clicked()
{
    ui->ok->setText("Login");
    ui->heading->setText("Log in");
    ui->loginWidget->show();
    isRegistration = false;
}


void StockMarket::on_close_clicked()
{
    ui->loginWidget->hide();
}

void StockMarket::alertsListener() {
    const std::lock_guard<std::mutex> lock(Global::mutex);
    Global::SendMessage(Global::s, Global::my_id, Requests::Check);
    auto x =  Global::ReadMessage(Global::s);
    if (x != "Bye!\n") {
        QMessageBox::information(this, "Transaction", x.c_str());
    }
}

void StockMarket::on_ok_clicked()
{
    Global::my_id = Global::ProcessRegistration(Global::s,
                                                ui->login->text().toStdString(), ui->password->text().toStdString(),
                                                isRegistration ? Requests::Registration : Requests::Login);
    if (Global::my_id == "0") {
        if (isRegistration) {
            QMessageBox::critical(this, "Unable to register",
                                  "This name has already been chosen by someone. Please, choose something else.");
        } else {
            QMessageBox::critical(this, "Unable to log in", "Invalid username or password.");
        }
    } else {
        QMessageBox::information(this, "Sucsess", "You have successfully logged in.");
        ui->welcome->hide();
        ui->main->show();
        timer->start(333);
    }
}


void StockMarket::on_exit_clicked()
{
    exit(0);
}

std::vector<std::string> split_string(const std::string& str, char c)
{
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};

    for (std::string line; std::getline(ss, line, c);)
        result.push_back(line);

    return result;
}


void StockMarket::on_UpdateList_clicked()
{
    const std::lock_guard<std::mutex> lock(Global::mutex);
    Global::SendMessage(Global::s, Global::my_id, Requests::ListActual);
    auto list = split_string(Global::ReadMessage(Global::s), '\n');
    ui->listWidget->clear();
    for (std::string& item : list) {
        new QListWidgetItem(QString::fromStdString(item), ui->listWidget);
    }
}



void StockMarket::on_AddOrder_clicked()
{
    Global::SendMessage(Global::s, Global::my_id, Requests::AddOrder, {
                                                  {"Quantity",  ui->Quantity->text().toStdString()},
                                                  {"Cost",      ui->Cost->text().toStdString()},
                                                               {"OrderType", (ui->sell->isChecked() ? "Sell" : "Buy")}
                                              });
    QMessageBox::information(this, "Info", Global::ReadMessage(Global::s).c_str());
}


void StockMarket::on_CanselOrder_clicked()
{
    const std::lock_guard<std::mutex> lock(Global::mutex);
    Global::SendMessage(Global::s, Global::my_id, Requests::Cansel, {{"Index", std::to_string(ui->listWidget->currentIndex().row() + 1)}});
    QMessageBox::information(this, std::to_string(ui->listWidget->currentIndex().row() + 1).c_str(), Global::ReadMessage(Global::s).c_str());
}


void StockMarket::on_UpdateQuotes_clicked()
{
    const std::lock_guard<std::mutex> lock(Global::mutex);
    Global::SendMessage(Global::s, Global::my_id, Requests::Quotes);
    auto res = split_string(Global::ReadMessage(Global::s), ' ');

    QtCharts::QLineSeries *series = new QtCharts::QLineSeries();

    for (size_t i = 0; i < res.size(); ++i) {
        series->append(i, std::stoi(res[i]));
    }
    series->setColor(QColor(0,0,255));
    series->setPointLabelsFormat("(@xPoint,@yPoint)");
    series->setPointsVisible(true);
    series->setName(QString("USD/RUB"));

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart-> createDefaultAxes();
    chart-> setTitleBrush (QBrush (QColor (255,170,255)));
    chart-> setTitleFont (QFont ("Microsoft Yahei"));
    chart-> setBackgroundVisible(false);
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setChart(chart);
    gridLayout->addWidget(chartView,0,0);
}


void StockMarket::on_UpdateBalance_clicked()
{
    const std::lock_guard<std::mutex> lock(Global::mutex);
    Global::SendMessage(Global::s, Global::my_id, Requests::Balance);
    ui->balance_lable->setText(QString::fromStdString(Global::ReadMessage(Global::s)));
}

