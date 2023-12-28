#include "mysniffer.h"
#include <QDebug>

MySniffer::MySniffer(QObject *parent = nullptr,QString ip = "0.0.0.0",int count = 0) : QThread(parent)
{
    this->ip = ip;
    this->count = count;
}

void MySniffer::setCount(int count)
{
    this->count = count;
}

void MySniffer::setIp(QString ip)
{
    this->ip = ip;
}

void MySniffer::stopThread()
{
    this->threadStop = true;
}

QString MySniffer::getDataFromPacket(BYTE *lpBuf, int iLen, int iPrintType)
{
    char*t = (char*)malloc(sizeof(char)*dwBufSize);
    int len =0;
    if (0 == iPrintType)   // 16进制
    {
        for (int i = 0; i < iLen; i++)
        {
//            if ((0 == (i % 8)) && (0 != i))
//            {
//                t[len++] =' ';
//            }
//            if ((0 == (i % 16)) && (0 != i))
//            {
//                t[len++] ='\n';
//            }
            len += sprintf(t+len,"%02x ",lpBuf[i]);
        }
//        t[len++] ='\n';
    }
    else if (1 == iPrintType) // ASCII编码
    {
        for (int i = 0; i < iLen; i++)
        {
            t[len++] =(char)lpBuf[i];
        }
    }

    t[len] ='\0';
    QString t_str(t);

    free(t);
    return t_str;
}

void MySniffer::analyseRecvPacket(BYTE *lpBuf)
{
    //Windows上没办法用Raw Socket抓MAC层的数据包，只能抓到IP层及以上的数据包！！！
    //注意：数据包的字节顺序转换问题！！！
    //这里要将网络字节序转换为本地字节序
    //分析IP协议
    //分析IP包的协议类型
    struct sockaddr_in saddr, daddr;
    PIPV4HEADER t_ip = (PIPV4HEADER)lpBuf;

    if(t_ip->ipv4_pro == IPPROTO_ICMP)
    {
        saddr.sin_addr.s_addr = t_ip->ipv4_sourpa;
        daddr.sin_addr.s_addr = t_ip->ipv4_destpa;

        emit this->sendData(QStringList()<<"ICMP"<<inet_ntoa(saddr.sin_addr)<<inet_ntoa(daddr.sin_addr)<<""<<"");
    }
    else if(t_ip->ipv4_pro == IPPROTO_IGMP)
    {

        saddr.sin_addr.s_addr = t_ip->ipv4_sourpa;
        daddr.sin_addr.s_addr = t_ip->ipv4_destpa;

        emit this->sendData(QStringList()<<"IGMP"<<inet_ntoa(saddr.sin_addr)<<inet_ntoa(daddr.sin_addr)<<""<<"");
    }
    else if(t_ip->ipv4_pro == IPPROTO_TCP)
    {
        PTCPHEADER tcp = (PTCPHEADER)(lpBuf + (t_ip->ipv4_ver_hl & 0x0F)*4);
        int hlen = ((t_ip->ipv4_ver_hl & 0x0F) * 4) + ((((tcp->tcp_hlen)>>4 & 0x0F) * 4)& 0x0F);
        int dlen = ((quint16)ntohs(t_ip->ipv4_plen )) - hlen;    //这里要将网络字节序转换为本地字节序
        saddr.sin_addr.s_addr = t_ip->ipv4_sourpa;
        daddr.sin_addr.s_addr = t_ip->ipv4_destpa;

        QStringList string_tcp;
        string_tcp<<"TCP";
        string_tcp <<QString(inet_ntoa(saddr.sin_addr))+":"+QString::number(ntohs(tcp->tcp_sourport));
        string_tcp <<QString(inet_ntoa(daddr.sin_addr))+":"+QString::number(ntohs(tcp->tcp_destport));

        string_tcp<<getDataFromPacket((lpBuf + hlen), dlen, 0);
        //MyPrintf("ack:%u  syn:%u length=%d\n", tcp->tcp_acknu, tcp->tcp_seqnu, dlen);
        string_tcp <<QString("ack:%1  syn:%2 length=%3").arg(tcp->tcp_acknu).arg(tcp->tcp_seqnu).arg(dlen);
        emit this->sendData(string_tcp);
    }
    else if(t_ip->ipv4_pro == IPPROTO_UDP)
    {
        PUDPHEADER udp = (PUDPHEADER)(lpBuf + (t_ip->ipv4_ver_hl & 0x0F) * 4);
        int hlen = (int)((t_ip->ipv4_ver_hl & 0x0F) * 4 + sizeof(UDPHEADER));
        int dlen = (int)(ntohs(udp->udp_hlen) - 8);
        //	int dlen = (int)(udp->udp_hlen - 8);
        saddr.sin_addr.s_addr = t_ip->ipv4_sourpa;
        daddr.sin_addr.s_addr = t_ip->ipv4_destpa;

        QStringList string_udp;
        string_udp<<"UDP";
        string_udp <<QString(inet_ntoa(saddr.sin_addr))+":"+QString::number(ntohs(udp->udp_sourport));
        string_udp <<QString(inet_ntoa(daddr.sin_addr))+":"+QString::number(ntohs(udp->udp_destport));

        string_udp<<getDataFromPacket((lpBuf + hlen), dlen, 0);
        string_udp <<"";

        emit this->sendData(string_udp);
    }
    else
    {
        saddr.sin_addr.s_addr = t_ip->ipv4_sourpa;
        daddr.sin_addr.s_addr = t_ip->ipv4_destpa;

        emit this->sendData(QStringList()<<"OTHER"<<inet_ntoa(saddr.sin_addr)<<inet_ntoa(daddr.sin_addr)<<""<<"");

    }

}

void MySniffer::run()
{
    this->threadStop = false;

    // 初始化Winsock服务环境, 设置版本
    WSADATA wsaData;
    if(0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        emit this->sendError("WSAStartup:\n" + GetLastError());
        return;
    }

    // 创建原始套接字
    // ！！！Windows上没办法用Raw Socket抓MAC层的数据包，只能抓到IP层及以上的数据包！！！
    //创建SOCK_RAW需要管理员权限
    SOCKET g_RawSocket;
    g_RawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (INVALID_SOCKET == g_RawSocket)
    {
        WSACleanup();
        emit this->sendError("socket:\n" + GetLastError());
        return ;
    }

    // 构造地址结构
    sockaddr_in SockAddr;
    RtlZeroMemory(&SockAddr, sizeof(sockaddr_in));
    SockAddr.sin_addr.S_un.S_addr = inet_addr(this->ip.toUtf8().data());
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(0);
    // 绑定
    if (SOCKET_ERROR == bind(g_RawSocket, (sockaddr *)(&SockAddr), sizeof(sockaddr_in)))
    {
        closesocket(g_RawSocket);
        WSACleanup();
        emit this->sendError("bind:\n" + GetLastError());
        return ;
    }

    // 设置混杂模式，这样才能捕获所有的数据包
    DWORD dwSetVal = 1;
    if (SOCKET_ERROR == ioctlsocket(g_RawSocket, SIO_RCVALL, &dwSetVal))
    {
        closesocket(g_RawSocket);
        WSACleanup();
        emit this->sendError("ioctlsocket:\n" + GetLastError());
        return ;
    }

    sockaddr_in RecvAddr;
    RtlZeroMemory(&RecvAddr, sizeof(sockaddr_in));
    int iRecvBytes = 0;
    int iRecvAddrLen = sizeof(sockaddr_in);
    BYTE *lpRecvBuf = new BYTE[dwBufSize];

    while(!threadStop && count--)
    {
        RtlZeroMemory(&RecvAddr, sizeof(sockaddr_in));
        iRecvBytes = recvfrom(g_RawSocket, (char *)lpRecvBuf, dwBufSize, 0, (sockaddr *)(&RecvAddr), &iRecvAddrLen);
        if (0 < iRecvBytes)
        {
            // 接收到数据包
            // 分析数据包
            analyseRecvPacket(lpRecvBuf);
        }
    }

    closesocket(g_RawSocket);
    WSACleanup();

}
