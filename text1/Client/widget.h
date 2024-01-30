#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>  //客户端头文件
#include <QMessageBox> //

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_connectBtn_clicked();
    void connected_slot();        //自定义处理connected信号的槽函数
    void readyRead_slot();        //自定义处理readyRead信号的槽函数
    void on_sendBtn_clicked();    //自定义处理发送按钮的槽函数
    void disconnect_slot();       //自定义处理disconnectFromHost信号的函数
    void on_disConnectBtn_clicked();

private:
    Ui::Widget *ui;

    //定义一个客户端指针
    QTcpSocket *socket;
    //用户名
    QString userName;     //由于多个槽函数需要用到该用户名，所以将用户名设置为类的私有成员

};
#endif // WIDGET_H
