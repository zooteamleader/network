#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("计算机网络课程设计-简单 Web Server 程序的设计与实现");
    this->setWindowIcon(QIcon(":/image/photo.jpg"));


    initExp8();

}

MainWindow::~MainWindow()
{
    delete ui;
}

