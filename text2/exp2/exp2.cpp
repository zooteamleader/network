#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QNetworkInterface>//网卡信息
#include <QHostInfo>//计算机名
#include <QNetworkAddressEntry>
#include <QList>
void MainWindow::initExp2()
{

    ui->tableWidget_exp2_pingResult->setColumnCount(4);
    ui->tableWidget_exp2_pingResult->setHorizontalHeaderLabels(QStringList()<<"IP地址" << "网卡地址"<< "主机名"<<"返回信息");
    ui->tableWidget_exp2_pingResult->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tableWidget_exp2_pingResult->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableWidget_exp2_pingResult->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    ui->tableWidget_exp2_pingResult->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    exp2_pingResultCount = 0;

    myPing = new MyPing(this,QStringList(DEFAULT_IP),ui->lineEdit_exp2_pingTime->text().toInt(),ui->lineEdit_exp2_pingCount->text().toInt());
    connect(myPing,&MyPing::sendResult,this,&MainWindow::exp2_recvPingResult);
    connect(myPing,&MyPing::finished,this,&MainWindow::exp2_myPingFinished);
    connect(this,&MainWindow::stopPingThread,myPing,&MyPing::stopThread);

}


void MainWindow::exp2_myPingError(QString data)
{
    QMessageBox::critical(this,"Ping错误",data);
}

void MainWindow::on_pushButton_exp2_pingStart_clicked()
{
    if(ui->pushButton_exp2_pingStart->text() =="开始")
    {
        if(ui->lineEdit_exp2_pingIP4->text().toInt() > ui->lineEdit_exp2_pingIP5->text().toInt())
        {
             QMessageBox::critical(this,"输入错误","起止范围错误");
             return ;
        }

        QString ip_start(""),ip_end;
        ip_start += ui->lineEdit_exp2_pingIP1->text();
        ip_start += ".";
        ip_start += ui->lineEdit_exp2_pingIP2->text();
        ip_start += ".";
        ip_start += ui->lineEdit_exp2_pingIP3->text();
        ip_start += ".";
        ip_end = QString(ip_start);
        ip_start += ui->lineEdit_exp2_pingIP4->text();
        ip_end += ui->lineEdit_exp2_pingIP5->text();

        if(!MyPing::isLegalIP(ip_start))
        {
            QMessageBox::critical(this,"输入错误","起始IP地址不合法");
            return ;
        }

        if(!MyPing::isLegalIP(ip_end))
        {
            QMessageBox::critical(this,"输入错误","结束IP地址不合法");
            return ;
        }
        QStringList ipList;

        for(int i=ui->lineEdit_exp2_pingIP4->text().toInt();i<=ui->lineEdit_exp2_pingIP5->text().toInt();++i)
        {
            QString t = QString("");
            t += ui->lineEdit_exp2_pingIP1->text();
            t += ".";
            t += ui->lineEdit_exp2_pingIP2->text();
            t += ".";
            t += ui->lineEdit_exp2_pingIP3->text();
            t += ".";
            t += QString::number(i);

            ipList<<t;
        }//获取ip信息

        myPing->setIpList(ipList);
        myPing->setTimeout(ui->lineEdit_exp2_pingTime->text().toInt());
        myPing->setCount(ui->lineEdit_exp2_pingCount->text().toInt());

        myPing->start();

        ui->pushButton_exp2_pingStart->setText("停止");
        ui->widget_exp2_pingIP->setEnabled(false);
        ui->lineEdit_exp2_pingCount->setEnabled(false);
        ui->lineEdit_exp2_pingTime->setEnabled(false);

        ui->tableWidget_exp2_pingResult->clear();
        ui->tableWidget_exp2_pingResult->setColumnCount(4);
        ui->tableWidget_exp2_pingResult->setHorizontalHeaderLabels(QStringList()<<"IP地址"<< "网卡地址"<< "主机名"<< "返回信息");
        exp2_pingResultCount = 0;

    }

    else if(ui->pushButton_exp2_pingStart->text() =="停止")
    {
        emit this->stopPingThread();

        ui->pushButton_exp2_pingStart->setText("开始");
        ui->widget_exp2_pingIP->setEnabled(true);
        ui->lineEdit_exp2_pingCount->setEnabled(true);
        ui->lineEdit_exp2_pingTime->setEnabled(true);
    }

}

void MainWindow::exp2_recvPingResult(QString data)
{
    //qDebug()<<data;//ping信息
    if(ui->pushButton_exp2_pingStart->text() == "开始")
        return ;


    QStringList result = data.split('#');
    if(result[1] == "*")
    {

        exp2_pingResultCount ++;
          ui->tableWidget_exp2_pingResult->setRowCount(exp2_pingResultCount);
          ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,0,new QTableWidgetItem(result[0]));
          ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,3,new QTableWidgetItem("..."));//返回信息在等待中
          ui->tableWidget_exp2_pingResult->scrollToBottom();

          // 获取IP地址对应的网卡地址和主机名
          QString ipAddr = result[0];
          QHostAddress hostAddr(ipAddr);
          QString macAddr = "";
          QString hostName = "";
         QList<QNetworkInterface>list = QNetworkInterface::allInterfaces();//获取所有网络接口的列表
         foreach(QNetworkInterface netinterface, list) {//foreach循环遍历每个网络接口
             QList<QNetworkAddressEntry> entryList = netinterface.addressEntries();
             foreach (QNetworkAddressEntry entry, entryList) {//foreach循环遍历当前接口的地址条目列表
                 if(entry.ip() == hostAddr){
                     macAddr = netinterface.hardwareAddress();//网卡地址
                     hostName = QHostInfo::localHostName();//主机名
                     break;
                 }
              }
          }

          // 将网卡地址和主机名填充到表格中
          ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,1,new QTableWidgetItem(macAddr));
          ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,2,new QTableWidgetItem(hostName));
    }
    else if(result[1] == "在线" || result[1] == "超时")
    {
        ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,3,new QTableWidgetItem(result[1]));
    }

}

void MainWindow::exp2_myPingFinished()
{
   ui->pushButton_exp2_pingStart->setText("开始");
   ui->widget_exp2_pingIP->setEnabled(true);
   ui->lineEdit_exp2_pingCount->setEnabled(true);
   ui->lineEdit_exp2_pingTime->setEnabled(true);
}

