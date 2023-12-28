#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("计算机网络课程设计-基于 IP 多播的网络会议程序");
    this->setWindowIcon(QIcon(":/image/photo.jpg"));


    initExp3();


}

MainWindow::~MainWindow()
{
    delete ui;
}

