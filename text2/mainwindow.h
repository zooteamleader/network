#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QNetworkInterface>//网卡信息
#include <QHostInfo>//计算机名
#include "exp2/mytracert.h"
#include "exp2/myping.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    const QString DEFAULT_IP = QString("39.101.201.13");

signals:


    void stopPingThread();

    void stopMulticastThread();

    void stopSnifferThread();

private slots:


    //实验二

    void on_pushButton_exp2_pingStart_clicked();

    void exp2_recvPingResult(QString data);

    void exp2_myPingFinished();

    void exp2_myPingError(QString data);



private:
    Ui::MainWindow *ui;
QMap<QString, QString> hostInfoMap;

    //实验二
    MyPing* myPing;
    int exp2_pingResultCount = 0;
    int exp2_pingCount = 0;
    void initExp2();




};

#endif // MAINWINDOW_H
