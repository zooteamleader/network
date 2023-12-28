#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

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
    //实验一
    void on_pushButton_exp1_login_clicked();

    void on_pushButton_exp1_reset_clicked();

    void on_pushButton_exp1_clear_clicked();

    void on_pushButton_exp1_send_clicked();

    void on_listWidget_exp1_userList_clicked(const QModelIndex &index);

    void on_pushButton_exp1_startExe_clicked();

    void exp1_Socket_Read_Data();

    void exp1_Socket_Disconnected();



private:
    Ui::MainWindow *ui;
    //实验1
    bool exp1_login = false;
    int exp1_count_accept = 0;
    int exp1_count_send = 0;
    QString exp1_ip;
    int exp1_post;
    QTcpSocket* exp1_socket;
    int exp1_person = 0;
    int exp1_person_index = 0;
    void initExp1();



};

#endif // MAINWINDOW_H
