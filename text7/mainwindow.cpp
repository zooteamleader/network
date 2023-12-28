#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("计算机网络课程设计-网络嗅探器的设计与实现");
    this->setWindowIcon(QIcon(":/image/photo.jpg"));


    initExp4();


}

MainWindow::~MainWindow()
{
    delete ui;
}

