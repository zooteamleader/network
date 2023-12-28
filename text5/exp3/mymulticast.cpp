
#include "mymulticast.h"
#include <QTextCodec>

MyMulticast::MyMulticast(QObject *parent = nullptr,QString destIP =NULL,QString localIP = NULL,int port = 23333) : QThread(parent)
{
    this->remoteIP = destIP;
    this->localIP = localIP;
    this->port = port;

    loginFlag = false;
}

void MyMulticast::setRemoteIP(QString destIP)
{
    this->remoteIP = destIP;
}

void MyMulticast::setLocalIP(QString localIP)
{
    this->localIP = localIP;
}

void MyMulticast::setPort(int port)
{
    this->port = port;
}

bool MyMulticast::isLegalIP(QString ip)
{
    WSADATA* wsa =(WSADATA*) malloc(sizeof(WSADATA));
    WSAStartup(MAKEWORD(2,2),wsa);
    bool flag = true;
    //得到 IP 地址
    u_long ulDestIP=inet_addr(ip.toUtf8().data());
    //转换不成功
    if(ulDestIP==INADDR_NONE)
    {
        flag = false;
    }
    free(wsa);

    return flag;
}

void MyMulticast::stopThread()
{
    this->threadStop = true;
}

void MyMulticast::run()
{
    loginFlag = false;
    threadStop = false;

    WSADATA wsd;
    int len = sizeof( struct sockaddr_in);

    //初始化 WinSock2.2
    if( WSAStartup( MAKEWORD(2,2),&wsd) != 0 )
    {
        emit this->sendError("WSAStartup() failed");
        threadStop = true;
        return;
    }

    if((sock=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,
                       WSA_FLAG_MULTIPOINT_C_LEAF|WSA_FLAG_MULTIPOINT_D_LEAF|
                       WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        emit this->sendError(QString("socket failed with:\n%1").arg(WSAGetLastError()));
        threadStop = true;
        WSACleanup();
        return;
    }

    //绑定本地ip
    local.sin_family = AF_INET;
    local.sin_port = htons(this->port);
    local.sin_addr.s_addr = inet_addr(this->localIP.toUtf8().data());
    if( bind(sock,(struct sockaddr*)&local,sizeof(local)) == SOCKET_ERROR )
    {
        emit this->sendError(QString("bind failed with:\n%1").arg(WSAGetLastError()));
        threadStop = true;
        closesocket(sock);
        WSACleanup();
        return ;
    }

    //加入多播组
    remote.sin_family = AF_INET;
    remote.sin_port = htons(this->port);
    remote.sin_addr.s_addr = inet_addr(this->remoteIP.toUtf8().data());
    if(( sockM = WSAJoinLeaf(sock,(SOCKADDR*)&remote,
                             sizeof(remote),NULL,NULL,NULL,NULL,
                             JL_BOTH)) == INVALID_SOCKET)
    {
        emit this->sendError(QString("WSAJoinLeaf() failed:\n%1").arg(WSAGetLastError()));
        threadStop = true;
        closesocket(sock);
        WSACleanup();
        return ;
    }

    loginFlag = true;
    int ret = 0;
    char recvbuf[BUFSIZE];
//    TCHAR recvbuf[BUFSIZE];

    //接受数据
    while(!threadStop)
    {

        if(( ret = recvfrom(sock,recvbuf,BUFSIZE,0,(struct sockaddr*)&from,&len)) == SOCKET_ERROR)
        {
            emit this->sendError(QString("recvfrom failed with:\n%1").arg(WSAGetLastError()));
            threadStop = true;
            closesocket(sockM);
            closesocket(sock);
            WSACleanup();
            return ;
        }

        recvbuf[ret] = '\0';    //GBK数据
//        emit this->recvData(QString("RECV:' %1 ' FROM <%2>").arg(QString::fromWCharArray(recvbuf)).arg(inet_ntoa(from.sin_addr)));

        //gbk -> utf8
        //1. gbk to unicode
        QTextCodec* gbk = QTextCodec::codecForName("gbk");
        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(utf8);
        QString strUnicode = gbk->toUnicode(recvbuf);
        //2. unicode -> utf-8
        QByteArray utf8_bytes=utf8->fromUnicode(strUnicode);
        char *recvbuf_utf8  = utf8_bytes.data(); //获取其char *

        emit this->recvData(QString("%1 SEND :\n%2").arg(inet_ntoa(from.sin_addr)).arg(recvbuf_utf8));
    }

    closesocket(sockM);
    closesocket(sock);
    WSACleanup();
}

void MyMulticast::sendData(QString data)
{
    if(loginFlag)
    {
//        char * sendbuf = data.toUtf8().data();
        //utf8 -> gbk
        //1. utf8 -> unicode
        QTextCodec* gbk = QTextCodec::codecForName("gbk");
        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(utf8);
        QString strUnicode= utf8->toUnicode(data.toLocal8Bit().data());
        //2. unicode -> gbk, 得到QByteArray
        QByteArray gb_bytes= gbk->fromUnicode(strUnicode);
        char * sendbuf =  gb_bytes.data(); //获取其char *


        if( sendto(sockM,(char*)sendbuf,strlen(sendbuf),0,(struct sockaddr*)
                   &remote,sizeof(remote))==SOCKET_ERROR)
        {
            emit this->sendError(QString("sendto failed with: \n%1").arg(WSAGetLastError()));
            threadStop = true;
            closesocket(sockM);
            closesocket(sock);
            WSACleanup();
            return ;
        }

    }
    else
    {
        emit this->sendError(QString("请先登录"));
    }
}
