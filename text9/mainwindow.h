#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "exp9/myserver.h"

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
    void stopTracertThread();

    void stopPingThread();

    void stopMulticastThread();

    void stopSnifferThread();

private slots:


    //实验九 简单 Web Server 程序的设计与实现
    void on_pushButton_exp9_start_clicked();

    void exp9_myServerError(QString error);

    void exp9_recvServerData(QString data);

private:
    Ui::MainWindow *ui;


    //实验九
    MyServer* myServer;
    void initExp9();


};

#endif // MAINWINDOW_H
