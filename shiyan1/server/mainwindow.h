#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>  //服务器头文件
#include <QTcpSocket>  //客户端头文件
#include <QList>       //链表头文件，用于存放客户端容器
#include <QMessageBox> //消息对话框类
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_startBtn_clicked(); //自定义启动按钮的槽函数
        void newConnection_slot();  //自定义处理newConnection信号的槽函数
        void readyRead_slot();      //自定义处理reayRead信号的槽函数
        void on_Widget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    //定义服务器指针
       QTcpServer *server;
       //定义客户端指针链表容器
       QList<QTcpSocket *> clientList;
};

#endif // MAINWINDOW_H
