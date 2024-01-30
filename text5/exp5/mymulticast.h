#ifndef MYMULTICAST_H
#define MYMULTICAST_H

#include <QThread>
#include <winsock2.h>
#include <ws2tcpip.h>

#define BUFSIZE 10240 //发送数据缓冲大小

class MyMulticast : public QThread
{
    Q_OBJECT
public:
    explicit MyMulticast(QObject *parent,QString remoteIP,QString localIP,int port);

    void setRemoteIP(QString remoteIP);
    void setLocalIP(QString localIP);
    void setPort(int port);

    void run();

    static bool isLegalIP(QString ip);

    bool threadStop = false;
    bool loginFlag = false;

signals:
    void recvData(QString data);
    void sendError(QString data);

public slots:
    void sendData(QString data);

    void stopThread();

private:
    QString remoteIP;
    QString localIP;
    int port;

    SOCKET sock,sockM;
    struct sockaddr_in local,remote,from;

};

#endif // MYMULTICAST_H
