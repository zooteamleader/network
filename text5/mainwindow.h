#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

#include "exp3/mymulticast.h"


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

    //实验三
    void exp3_myMulticastError(QString data);

    void exp3_myMulticastFinished();

    void exp3_recvMulticastData(QString data);

    void on_pushButton_exp3_start_clicked();

    void on_pushButton_exp3_send_clicked();



private:
    Ui::MainWindow *ui;


    //实验三
    MyMulticast* myMulticast;
    void initExp3();




};

#endif // MAINWINDOW_H
