#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

#include "exp4/mysniffer.h"


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


    //实验四
    void exp4_mySnifferFinish();

    void exp4_recvSnifferData(QStringList data);

    void exp4_mySnifferError(QString data);

    void on_pushButton_exp4_start_clicked();

    void on_pushButton_exp4_saveToFile_clicked();



private:
    Ui::MainWindow *ui;


    //实验四
    MySniffer* mySniffer;
    QList<QStringList> dataList;
    void initExp4();




};

#endif // MAINWINDOW_H
