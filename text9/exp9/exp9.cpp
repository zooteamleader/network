#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QFile>


void MainWindow::initExp9()
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //    qDebug()<<ipAddressesList;
    foreach (QHostAddress address, ipAddressesList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            ui->comboBox_exp9_localIp->addItem(address.toString());
        }
    }

    myServer = new MyServer(this,ui->comboBox_exp9_localIp->currentText(),ui->lineEdit_exp9_port->text().toInt(),ui->lineEdit_exp9_fileName->text());

    connect(myServer,&MyServer::sendError,this,&MainWindow::exp9_myServerError);
    connect(myServer,&MyServer::sendData,this,&MainWindow::exp9_recvServerData);

}

void MainWindow::on_pushButton_exp9_start_clicked()
{
    if(ui->pushButton_exp9_start->text() == "启动服务器")
    {
        myServer->localIp = ui->comboBox_exp9_localIp->currentText();
        myServer->port = ui->lineEdit_exp9_port->text().toInt();
        myServer->fileName = ui->lineEdit_exp9_fileName->text();

        if(myServer->startServer())
        {
            ui->textBrowser_exp9_debug->setText("");
            ui->pushButton_exp9_start->setText("关闭服务器");
            ui->comboBox_exp9_localIp->setEnabled(false);
            ui->lineEdit_exp9_port->setEnabled(false);
            ui->lineEdit_exp9_fileName->setEnabled(false);
QString link=QString("http://%1:%2/").arg(myServer->localIp).arg(myServer->port);
 //ui->textBrowser_exp9_debug->setText(link);
ui->textBrowser_exp9_debug->setHtml(QString("<a href=\"%1\">%1</a>").arg(link));
ui->textBrowser_exp9_debug->setOpenExternalLinks(true);

        }
    }
    else if(ui->pushButton_exp9_start->text() == "关闭服务器")
    {
        myServer->close();
        ui->pushButton_exp9_start->setText("启动服务器");
        ui->comboBox_exp9_localIp->setEnabled(true);
        ui->lineEdit_exp9_port->setEnabled(true);
        ui->lineEdit_exp9_fileName->setEnabled(true);
    }
}


void MainWindow::exp9_myServerError(QString error)
{
     QMessageBox::critical(this,"错误",error);
}

void MainWindow::exp9_recvServerData(QString data)
{
    ui->textBrowser_exp9_debug->append(data);


}

