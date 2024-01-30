#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QHostAddress>

void MainWindow::initExp5()
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //    返回主机上所有Ip地址的列表
    foreach (QHostAddress address, ipAddressesList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ui->comboBox_exp5_localIp->addItem(address.toString());
        }
    }

    myMulticast = new MyMulticast(this,ui->lineEdit_exp5_remoteIP->text(),ui->comboBox_exp5_localIp->currentText(),ui->lineEdit_exp5_port->text().toInt());
    connect(myMulticast,&MyMulticast::finished,this,&MainWindow::exp5_myMulticastFinished);
    connect(myMulticast,&MyMulticast::recvData,this,&MainWindow::exp5_recvMulticastData);
    connect(myMulticast,&MyMulticast::sendError,this,&MainWindow::exp5_myMulticastError);
    connect(this,&MainWindow::stopMulticastThread,myMulticast,&MyMulticast::stopThread);

}

void MainWindow::exp5_myMulticastError(QString data)
{
    QMessageBox::critical(this,"Multicast错误",data);
}

void MainWindow::exp5_myMulticastFinished()
{
    ui->pushButton_exp5_start->setText("启动");
    ui->lineEdit_exp5_remoteIP->setEnabled(true);
    ui->lineEdit_exp5_port->setEnabled(true);
    ui->comboBox_exp5_localIp->setEnabled(true);
}

void MainWindow::exp5_recvMulticastData(QString data)
{
    ui->textBrowser_exp5_content->append(data);
}
//开始启动
void MainWindow::on_pushButton_exp5_start_clicked()
{
    if(ui->pushButton_exp5_start->text() == "启动")
    {
        if(!MyMulticast::isLegalIP(ui->lineEdit_exp5_remoteIP->text()))
        {
            QMessageBox::critical(this,"不合法","不合法");
            return ;
        }

        myMulticast->setLocalIP(ui->comboBox_exp5_localIp->currentText());
        myMulticast->setPort(ui->lineEdit_exp5_port->text().toInt());
        myMulticast->setRemoteIP(ui->lineEdit_exp5_remoteIP->text());

        myMulticast->start();

        ui->pushButton_exp5_start->setText("关闭");
        ui->lineEdit_exp5_remoteIP->setEnabled(false);
        ui->lineEdit_exp5_port->setEnabled(false);
        ui->comboBox_exp5_localIp->setEnabled(false);
        ui->textBrowser_exp5_content->setText("");

    }
    else if(ui->pushButton_exp5_start->text() == "关闭")
    {
        emit this->stopMulticastThread();

        ui->pushButton_exp5_start->setText("启动");
        ui->lineEdit_exp5_remoteIP->setEnabled(true);
        ui->lineEdit_exp5_port->setEnabled(true);
        ui->comboBox_exp5_localIp->setEnabled(true);
    }



}

void MainWindow::on_pushButton_exp5_send_clicked()
{
    if(ui->pushButton_exp5_start->text() == "启动")
    {
        QMessageBox::critical(this,"错误","请先启动");

    }
    else if(ui->pushButton_exp5_start->text() == "关闭")
    {
        myMulticast->sendData(ui->textEdit_exp5_send->toPlainText());
        ui->textBrowser_exp5_content->append(ui->comboBox_exp5_localIp->currentText() + " SEND :");
        ui->textBrowser_exp5_content->append(ui->textEdit_exp5_send->toPlainText());
        ui->textEdit_exp5_send->setText("");
    }
}
