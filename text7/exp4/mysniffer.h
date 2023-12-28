#ifndef MYSNIFFER_H
#define MYSNIFFER_H

#include <QThread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "exp4/packetstruct.h"

#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)

//和WinPcap相比,Raw Socket有很大的局限性,它只能抓到IP层及以上的数据包,抓不到MAC层的数据包
//https://blog.csdn.net/demongwc/article/details/85258599
//https://www.writebug.com/git/codes?owner=Schoolleave&repo=RawSocket_Test

class MySniffer : public QThread
{
    Q_OBJECT
public:
    explicit MySniffer(QObject *parent,QString ip ,int count);

    void setIp(QString ip);
    void setCount(int count);

    bool threadStop = false;

    void run();

signals:
    void sendData(QStringList data);
    void sendError(QString data);

public slots:
    void stopThread();


private:
     QString ip;
     int count = 0;

     const DWORD dwBufSize = 10240;
     QString getDataFromPacket(BYTE *lpBuf, int iLen, int iPrintType);
     void analyseRecvPacket(BYTE *lpBuf);

};

#endif // MYSNIFFER_H
