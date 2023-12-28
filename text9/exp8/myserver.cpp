#include "myserver.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFile>
#include <QDir>
#include <QDateTime>

MyServer::MyServer(QObject *parent,QString localIp,int port,QString fileName) : QObject(parent)
{
    this->localIp = localIp;
    this->port = port;
    this->fileName = fileName;

    tcpServer = new QTcpServer;

    connect(tcpServer,&QTcpServer::newConnection,this,&MyServer::newConnection);
}

bool MyServer::startServer()
{
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //    qDebug()<<ipAddressesList;
    foreach (QHostAddress address, ipAddressesList)
    {
        if(address.toString() == localIp)
        {
            if(!tcpServer->listen(address,port))
            {
                emit this->sendError("Listen Fail");
                return false;
            }
            return true;
        }
    }
    return false;
}

void MyServer::newConnection()
{
    MySocket* mySocket = new MySocket(tcpServer->nextPendingConnection());
    connect(mySocket,&MySocket::socketReadyRead,this,&MyServer::socketReadyRead);

    mySocketList.push_back(mySocket);


}

void MyServer::socketReadyRead(QTcpSocket *socket, QString data)
{
    this->sendData(data);
    QString path =  data.split(" ").at(1);
    int result = -1;

    if(path == "/")     //访问根路径
    {
        QFile file(QDir::currentPath()+"/" +fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit this->sendError(QDir::currentPath()+"/" + fileName +" 打开失败");
            result = socket->write(getHeaderString(false).toUtf8());
            return;
        }

        QTextStream in(&file);
        QString fileData = in.readAll();
        result = socket->write((getHeaderString(true).arg(fileData.size()) + fileData +QString("\r\n")).toUtf8());
        this->sendData("Server Response:\r\n"+ getHeaderString(true).arg(fileData.size()) + fileData +"\r\n");
//        this->sendData("Server Response:\r\n"+ getHeaderString(true) + fileData);
    }
    else
    {
        this->sendData("Server Response:\r\n"+ getHeaderString(false).arg(0));
        result = socket->write(getHeaderString(false).arg(0).toUtf8());
    }

    socket->flush();

    if(result == -1)
    {
        emit this->sendError("传输失败");
        return;
    }

//    socket->close();
}

void MyServer::close()
{
    foreach (MySocket* mySocket, mySocketList) {
        mySocket->close();
    }

    mySocketList.clear();
    tcpServer->close();

}

QString MyServer::getHeaderString(bool state)
{
    //HTTP/1.1 200 OK
    //Date: Sat, 31 Dec 2005 23:59:59 GMT
    //Content-Type: text/html;charset=ISO-8859-1
    //Content-Length: %1
    QString header = "";
    if(state == true)
    {
        header = header + "HTTP/1.1 200 OK" + "\r\n";
        header = header + "Date: ";
        header = header + QDateTime::currentDateTime().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT\r\n";
        header = header + "Content-Type: text/html;charset=ISO-8859-1" + "\r\n";
        header = header + "Content-Length: %1" + "\r\n";
    }
    else if(state == false)
    {
        header = header + "HTTP/1.1 404 Not Found" + "\r\n";
        header = header + "Date: ";
        header = header + QDateTime::currentDateTime().toString(Qt::RFC2822Date) + " GMT\r\n";
        header = header + "Content-Type: text/html;charset=ISO-8859-1" + "\r\n";
        header = header + "Content-Length: 0" + "\r\n";
    }
    return header;
}

