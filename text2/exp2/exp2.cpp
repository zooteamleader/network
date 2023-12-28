#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

void MainWindow::initExp2()
{
    ui->tableWidget_exp2_tracertResult->setColumnCount(2);
    ui->tableWidget_exp2_tracertResult->setHorizontalHeaderLabels(QStringList()<<"时间"<< "IP地址");
    ui->tableWidget_exp2_tracertResult->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableWidget_exp2_tracertResult->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    exp2_tracertResultCount = 0;

    myTracert = new MyTracert(this,ui->lineEdit_exp2_tracertIP->text(),ui->lineEdit_exp2_tracertTime->text().toInt(),ui->lineEdit_exp2_tracertCount->text().toInt());
    connect(myTracert,&MyTracert::sendResult,this,&MainWindow::exp2_recvTracertResult);
    connect(myTracert,&MyTracert::finished,this,&MainWindow::exp2_myTracertFinished);
    connect(this,&MainWindow::stopTracertThread,myTracert,&MyTracert::stopThread);

    ui->tableWidget_exp2_pingResult->setColumnCount(2);
    ui->tableWidget_exp2_pingResult->setHorizontalHeaderLabels(QStringList()<<"IP地址"<< "状态");
    ui->tableWidget_exp2_pingResult->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tableWidget_exp2_pingResult->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    exp2_pingResultCount = 0;

    myPing = new MyPing(this,QStringList(DEFAULT_IP),ui->lineEdit_exp2_pingTime->text().toInt(),ui->lineEdit_exp2_pingCount->text().toInt());
    connect(myPing,&MyPing::sendResult,this,&MainWindow::exp2_recvPingResult);
    connect(myPing,&MyPing::finished,this,&MainWindow::exp2_myPingFinished);
    connect(this,&MainWindow::stopPingThread,myPing,&MyPing::stopThread);

}

void MainWindow::on_pushButton_exp2_tracertStart_clicked()
{
    if(!MyTracert::isLegalIP(ui->lineEdit_exp2_tracertIP->text()))
    {
        QMessageBox::critical(this,"不合法","不合法");
        return ;
    }

    if(ui->pushButton_exp2_tracertStart->text() == "开始")
    {
        myTracert->setIP(ui->lineEdit_exp2_tracertIP->text());
        myTracert->setiTimeout(ui->lineEdit_exp2_tracertTime->text().toInt());
        myTracert->setDEF_MAX_HOP(ui->lineEdit_exp2_tracertCount->text().toInt());

        myTracert->start();

        ui->pushButton_exp2_tracertStart->setText("停止");
        ui->lineEdit_exp2_tracertCount->setEnabled(false);
        ui->lineEdit_exp2_tracertIP->setEnabled(false);
        ui->lineEdit_exp2_tracertTime->setEnabled(false);

        ui->tableWidget_exp2_tracertResult->clear();
        ui->tableWidget_exp2_tracertResult->setColumnCount(2);
        ui->tableWidget_exp2_tracertResult->setHorizontalHeaderLabels(QStringList()<<"时间"<< "IP地址");
        exp2_tracertResultCount = 0;




    }
    else if(ui->pushButton_exp2_tracertStart->text() == "停止")
    {
        emit stopTracertThread();

        ui->pushButton_exp2_tracertStart->setText("开始");
        ui->lineEdit_exp2_tracertCount->setEnabled(true);
        ui->lineEdit_exp2_tracertIP->setEnabled(true);
        ui->lineEdit_exp2_tracertTime->setEnabled(true);

    }

}

void MainWindow::exp2_recvTracertResult(QString data)
{
    if(ui->pushButton_exp2_tracertStart->text() == "开始")
        return ;

    exp2_tracertResultCount ++;
    QStringList result = data.split('#');
    ui->tableWidget_exp2_tracertResult->setRowCount(exp2_tracertResultCount);
    ui->tableWidget_exp2_tracertResult->setItem(exp2_tracertResultCount-1,0,new QTableWidgetItem(result[0]));
    ui->tableWidget_exp2_tracertResult->setItem(exp2_tracertResultCount-1,1,new QTableWidgetItem(result[1]));
    ui->tableWidget_exp2_tracertResult->scrollToBottom();
}

void MainWindow::exp2_myTracertFinished()
{

    ui->pushButton_exp2_tracertStart->setText("开始");
    ui->lineEdit_exp2_tracertCount->setEnabled(true);
    ui->lineEdit_exp2_tracertIP->setEnabled(true);
    ui->lineEdit_exp2_tracertTime->setEnabled(true);
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
        }

        myPing->setIpList(ipList);
        myPing->setTimeout(ui->lineEdit_exp2_pingTime->text().toInt());
        myPing->setCount(ui->lineEdit_exp2_pingCount->text().toInt());

        myPing->start();

        ui->pushButton_exp2_pingStart->setText("停止");
        ui->widget_exp2_pingIP->setEnabled(false);
        ui->lineEdit_exp2_pingCount->setEnabled(false);
        ui->lineEdit_exp2_pingTime->setEnabled(false);

        ui->tableWidget_exp2_pingResult->clear();
        ui->tableWidget_exp2_pingResult->setColumnCount(2);
        ui->tableWidget_exp2_pingResult->setHorizontalHeaderLabels(QStringList()<<"IP地址"<< "状态");
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
//    qDebug()<<data;
    if(ui->pushButton_exp2_pingStart->text() == "开始")
        return ;


    QStringList result = data.split('#');
    if(result[1] == "*")
    {
        exp2_pingResultCount ++;
        ui->tableWidget_exp2_pingResult->setRowCount(exp2_pingResultCount);
        ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,0,new QTableWidgetItem(result[0]));
        ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,1,new QTableWidgetItem("..."));
        ui->tableWidget_exp2_pingResult->scrollToBottom();
    }
    else if(result[1] == "YES" || result[1] == "NO")
    {
        ui->tableWidget_exp2_pingResult->setItem(exp2_pingResultCount-1,1,new QTableWidgetItem(result[1]));
    }

}

void MainWindow::exp2_myPingFinished()
{
   ui->pushButton_exp2_pingStart->setText("开始");
   ui->widget_exp2_pingIP->setEnabled(true);
   ui->lineEdit_exp2_pingCount->setEnabled(true);
   ui->lineEdit_exp2_pingTime->setEnabled(true);
}

