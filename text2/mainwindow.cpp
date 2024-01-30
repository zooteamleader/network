#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("计算机网络课程设计-Tracert 与 Ping 程序设计与实现");
    this->setWindowIcon(QIcon(":/image/photo.jpg"));


    initExp2();

}

MainWindow::~MainWindow()
{
    delete ui;
}

