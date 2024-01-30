#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

#include "exp5/mymulticast.h"


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

    //实验五
    void exp5_myMulticastError(QString data);

    void exp5_myMulticastFinished();

    void exp5_recvMulticastData(QString data);

    void on_pushButton_exp5_start_clicked();

    void on_pushButton_exp5_send_clicked();



private:
    Ui::MainWindow *ui;


    //实验五
    MyMulticast* myMulticast;
    void initExp5();




};

#endif // MAINWINDOW_H
