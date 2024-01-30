#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("计算机网络课程设计-网络聊天程序的设计与实现");
    this->setWindowIcon(QIcon(":/image/photo.jpg"));
    //将发送区域的组件设置为不可用状态
    ui->sendBtn->setEnabled(false);
    ui->msgEdit->setEnabled(false);
    ui->disConnectBtn->setEnabled(false);

    //给客户端指针实例化空间
    socket = new QTcpSocket(this);//创建QTcpSocket套接字对象

    //如果连接服务器成功，该客户端就会发射一个connected的信号
    //我们可以将该信号连接到自定义的槽函数中处理相关逻辑
    //由于该连接只需要连接一次，所以，写在构造函数中即可
    //添加槽函数以读取数据
    connect(socket,&QTcpSocket::connected,this,&Widget::connected_slot);

    //客户端与服务器连接成功后，如果服务器向客户端发来数据，那么该客户端就会自动发送一个readyRead信号
    //我们只需要
    connect(socket,&QTcpSocket::readyRead,this,&Widget::readyRead_slot);

    //当客户端与服务器断开连接后，该客户端就会发射一个disconnected的信号
    //我们可以将该信号与自定义的槽函数连接
    //由于只需要连接一次，所以将该连接写到构造函数中即可
    //点击断开连接按钮，
    connect(socket,&QTcpSocket::disconnected,this,&Widget::disconnect_slot);
}

Widget::~Widget()
{
    delete ui;
}

//连接服务器按钮对应的槽函数
void Widget::on_connectBtn_clicked()
{
    //获取ui界面的信息
    userName = ui->userNameEdit->text();           //获取用户名
    QString hostName = ui->ipEdit->text();         //获取主机地址
    quint16 port = ui->portEdit->text().toUInt();  //获取端口号

    //调用函数连接到主机
    //[virtual] void QAbstractSocket::connectToHost(const QString &hostName, quint16 port)
    //参数1：服务器的主机地址
    //参数2：端口号
    //返回值：无
    socket->connectToHost(hostName,port);//使用这个对象连接服务器,确定网络中的任何一个通讯节点。

    //如果连接服务器成功，该客户端就会发射一个connected的信号
    //我们可以将该信号连接到自定义的槽函数中处理相关逻辑
    //由于该连接只需要连接一次，所以，写在构造函数中即可
}

//关于处理connected信号的槽函数的定义
void Widget::connected_slot()
{
    QMessageBox::information(this,"成功","连接服务器成功");

    //设置组件的可用状态
    ui->msgEdit->setEnabled(true);
    ui->sendBtn->setEnabled(true);
    ui->disConnectBtn->setEnabled(true);
    ui->userNameEdit->setEnabled(false);
    ui->ipEdit->setEnabled(false);
    ui->portEdit->setEnabled(false);
    ui->connectBtn->setEnabled(false);

    //顺便向服务器发送一条消息，说：***：进入聊天室
    QString msg = userName + "：进入聊天室";
    socket->write(msg.toLocal8Bit());
}

//关于readyRead信号对应槽函数的实现
void Widget::readyRead_slot()
{
    //读取该客户端中的数据
    QByteArray msg = socket->readAll();

    //将数据展示在ui界面
    ui->msgList->addItem(QString::fromLocal8Bit(msg));

}

//发送按钮对应的槽函数
void Widget::on_sendBtn_clicked()
{
    //获取ui界面中的编辑的文本内容
    QString m = ui->msgEdit->text();

    //整合要发送的消息
    QString msg = userName + "：" + m;
    socket->write(msg.toLocal8Bit());

    //将消息编辑器中的内容清空
    ui->msgEdit->clear();
}

//断开服务器按钮对应的槽函数
void Widget::on_disConnectBtn_clicked()
{
    //准备要发送的信息
    QString msg = userName + "：离开聊天室";
    socket->write(msg.toLocal8Bit());

    //调用成员函数disconnectFromHost
    //函数原型：virtual void disconnectFromHost();
    //功能：断开客户端与服务器的连接
    //参数：无
    socket->disconnectFromHost();

    //当客户端与服务器断开连接后，该客户端就会发射一个disconnected的信号
    //我们可以将该信号与自定义的槽函数连接
    //由于只需要连接一次，所以将该连接写到构造函数中即可

}

//关于disconnectFromHost信号对应槽函数的实现
void Widget::disconnect_slot()
{
    QMessageBox::information(this,"退出","断开成功");
    //设置组件的可用状态
    ui->msgEdit->setEnabled(false);
    ui->sendBtn->setEnabled(false);
    ui->disConnectBtn->setEnabled(false);
    ui->userNameEdit->setEnabled(true);
    ui->ipEdit->setEnabled(true);
    ui->portEdit->setEnabled(true);
    ui->connectBtn->setEnabled(true);
}
