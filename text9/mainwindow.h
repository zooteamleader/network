#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "exp8/myserver.h"

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


    //实验八
    void on_pushButton_exp8_start_clicked();

    void exp8_myServerError(QString error);

    void exp8_recvServerData(QString data);

private:
    Ui::MainWindow *ui;


    //实验八
    MyServer* myServer;
    void initExp8();


};

#endif // MAINWINDOW_H
