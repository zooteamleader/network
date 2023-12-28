#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QFile>

void MainWindow::initExp8()
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //    qDebug()<<ipAddressesList;
    foreach (QHostAddress address, ipAddressesList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ui->comboBox_exp8_localIp->addItem(address.toString());
        }
    }

    myServer = new MyServer(this,ui->comboBox_exp8_localIp->currentText(),ui->lineEdit_exp8_port->text().toInt(),ui->lineEdit_exp8_fileName->text());

    connect(myServer,&MyServer::sendError,this,&MainWindow::exp8_myServerError);
    connect(myServer,&MyServer::sendData,this,&MainWindow::exp8_recvServerData);

}

void MainWindow::on_pushButton_exp8_start_clicked()
{
    if(ui->pushButton_exp8_start->text() == "启动服务器")
    {
        myServer->localIp = ui->comboBox_exp8_localIp->currentText();
        myServer->port = ui->lineEdit_exp8_port->text().toInt();
        myServer->fileName = ui->lineEdit_exp8_fileName->text();

        if(myServer->startServer())
        {
            ui->textBrowser_exp8_debug->setText("");
            ui->pushButton_exp8_start->setText("关闭服务器");
            ui->comboBox_exp8_localIp->setEnabled(false);
            ui->lineEdit_exp8_port->setEnabled(false);
            ui->lineEdit_exp8_fileName->setEnabled(false);

            ui->textBrowser_exp8_debug->setText(QString("http://%1:%2/").arg(myServer->localIp).arg(myServer->port));
        }
    }
    else if(ui->pushButton_exp8_start->text() == "关闭服务器")
    {
        myServer->close();
        ui->pushButton_exp8_start->setText("启动服务器");
        ui->comboBox_exp8_localIp->setEnabled(true);
        ui->lineEdit_exp8_port->setEnabled(true);
        ui->lineEdit_exp8_fileName->setEnabled(true);
    }
}


void MainWindow::exp8_myServerError(QString error)
{
     QMessageBox::critical(this,"错误",error);
}

void MainWindow::exp8_recvServerData(QString data)
{
    ui->textBrowser_exp8_debug->append(data);
}

