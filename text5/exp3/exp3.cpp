#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QHostAddress>

void MainWindow::initExp3()
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //    qDebug()<<ipAddressesList;
    foreach (QHostAddress address, ipAddressesList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ui->comboBox_exp3_localIp->addItem(address.toString());
        }
    }

    myMulticast = new MyMulticast(this,ui->lineEdit_exp3_remoteIP->text(),ui->comboBox_exp3_localIp->currentText(),ui->lineEdit_exp3_port->text().toInt());
    connect(myMulticast,&MyMulticast::finished,this,&MainWindow::exp3_myMulticastFinished);
    connect(myMulticast,&MyMulticast::recvData,this,&MainWindow::exp3_recvMulticastData);
    connect(myMulticast,&MyMulticast::sendError,this,&MainWindow::exp3_myMulticastError);
    connect(this,&MainWindow::stopMulticastThread,myMulticast,&MyMulticast::stopThread);

}

void MainWindow::exp3_myMulticastError(QString data)
{
    QMessageBox::critical(this,"Multicast错误",data);
}

void MainWindow::exp3_myMulticastFinished()
{
    ui->pushButton_exp3_start->setText("启动");
    ui->lineEdit_exp3_remoteIP->setEnabled(true);
    ui->lineEdit_exp3_port->setEnabled(true);
    ui->comboBox_exp3_localIp->setEnabled(true);
}

void MainWindow::exp3_recvMulticastData(QString data)
{
    ui->textBrowser_exp3_content->append(data);
}

void MainWindow::on_pushButton_exp3_start_clicked()
{
    if(ui->pushButton_exp3_start->text() == "启动")
    {
        if(!MyMulticast::isLegalIP(ui->lineEdit_exp3_remoteIP->text()))
        {
            QMessageBox::critical(this,"不合法","不合法");
            return ;
        }

        myMulticast->setLocalIP(ui->comboBox_exp3_localIp->currentText());
        myMulticast->setPort(ui->lineEdit_exp3_port->text().toInt());
        myMulticast->setRemoteIP(ui->lineEdit_exp3_remoteIP->text());

        myMulticast->start();

        ui->pushButton_exp3_start->setText("关闭");
        ui->lineEdit_exp3_remoteIP->setEnabled(false);
        ui->lineEdit_exp3_port->setEnabled(false);
        ui->comboBox_exp3_localIp->setEnabled(false);
        ui->textBrowser_exp3_content->setText("");

    }
    else if(ui->pushButton_exp3_start->text() == "关闭")
    {
        emit this->stopMulticastThread();

        ui->pushButton_exp3_start->setText("启动");
        ui->lineEdit_exp3_remoteIP->setEnabled(true);
        ui->lineEdit_exp3_port->setEnabled(true);
        ui->comboBox_exp3_localIp->setEnabled(true);
    }



}

void MainWindow::on_pushButton_exp3_send_clicked()
{
    if(ui->pushButton_exp3_start->text() == "启动")
    {
        QMessageBox::critical(this,"错误","请先启动");

    }
    else if(ui->pushButton_exp3_start->text() == "关闭")
    {
        myMulticast->sendData(ui->textEdit_exp3_send->toPlainText());
        ui->textBrowser_exp3_content->append(ui->comboBox_exp3_localIp->currentText() + " SEND :");
        ui->textBrowser_exp3_content->append(ui->textEdit_exp3_send->toPlainText());
        ui->textEdit_exp3_send->setText("");
    }
}
