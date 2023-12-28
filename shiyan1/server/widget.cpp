#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //在构造函数中给服务器指针实例化对象
    server = new QTcpServer(this);  //此时就创建一个服务器了
}

Widget::~Widget()
{
    delete ui;
}

//启动服务器按钮对应的槽函数
void Widget::on_startBtn_clicked()
{
    //1、获取ui界面上的端口号
    quint16 port = ui->portEdit->text().toUInt();

    //2、将服务器设置成监听状态
    //bool QTcpServer::listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0)
    //参数1：要监听的主机地址，如果是any，表示监听所有主机地址，也可以给特定的主机地址进行监听
    //参数2：通过指定的端口号进行访问服务器，如果是0，表示由服务器自动分配，如果非0，则表示指定端口号  (quint16 == unsignal int)
    //返回值：成功返回真，失败返回假
    if(!server->listen(QHostAddress::Any,port))
    {
        QMessageBox::critical(this,"失败","服务器启动失败");  //默认按钮为ok
        return;
    }else
    {
        QMessageBox::information(this,"成功","服务器启动成功");
    }
    //此时表明服务器启动成功，并对客户端连接进行监听
    //如果有客户端向服务器发来连接请求，那么该服务器就会自动发射一个newConnection
    //我们可以将该信号连接到对应的槽函数中处理相关逻辑
    connect(server,&QTcpServer::newConnection,this,&Widget::newConnection_slot);
}

//处理newConnection信号的槽函数实现
void Widget::newConnection_slot()
{
    qDebug()<<"有新的客户端发来连接请求了";

    //获取最新连接的客户端套接字
    //函数原型：[virtual] QTcpSocket *QTcpServer::nextPendingConnection()
    //参数：无
    //返回值：最新连接客户端套接字的指针
    QTcpSocket *s = server->nextPendingConnection();

    //将获取的套接字存放到客户端容器中
    clientList.push_back(s);

    //此时，客户端就和服务器建立起联系了
    //如果该套接字有数据向服务器发送过来，那么该套接字就会自动发射一个ready read信号
    //我们可以将该信号连接到自定义的槽函数中，然后处理相关逻辑
    connect(s, &QTcpSocket::readyRead, this, &Widget::readyRead_slot);
}

//关于readyRead信号对应槽函数的实现
void Widget::readyRead_slot()
{
    //删除客户端链表中的无效客户端套接字
    for(int i=0;i<clientList.count();i++)
    {
        //判断套接字的状态
        //函数原型：SocketState state() const;
        //功能：返回客户端套接字的状态
        //参数：无
        //返回值：客户端的状态，如果结果为0，表示未连接
        if(clientList[i]->state() == 0)
        {
            clientList.removeAt(i);     //将下标为i的客户端的套接字从链表中移除
        }
    }

    //遍历所有客户端，查看是哪个客户端发来数据
    for(int i=0 ; i<clientList.count() ; i++)
    {
        //函数原型：qint64 bytesAvailable() const override;
        //功能：返回当前客户端套接字中的可读数据字节个数
        //参数：无
        //返回值：当前客户端待读的字节数，如果该数据为0，表示无待读数据
        if(clientList[i]->bytesAvailable() != 0)
        {
            //读取当前客户端的相关数据，并返回一个字节数组
            //参数：无
            //返回值：数据的字节数组
            QByteArray msg = clientList[i]->readAll();

            //将数据展示到ui界面上
            ui->msgList->addItem(QString::fromLocal8Bit(msg));

            //将接收到的该消息，发送给所有客户端
            for(int j = 0 ; j<clientList.count() ; j++)
            {
                clientList[j]->write(msg);
            }
        }
    }
}

void Widget::on_Widget_customContextMenuRequested(const QPoint &pos)
{

}
