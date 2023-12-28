#include "mytracert.h"
#include <QDebug>
#include <cstring>

MyTracert::MyTracert(QObject *parent = nullptr,QString ip = NULL,int iTimeout = 3000,int DEF_MAX_HOP = 30): QThread(parent)
{
    this->ip = ip;
    this->iTimeout = iTimeout;
    this->DEF_MAX_HOP = DEF_MAX_HOP;

}

void MyTracert::setIP(QString ip)
{
    this->ip = ip;
}

void MyTracert::setiTimeout(int iTimeout)
{
    this->iTimeout = iTimeout;
}

void MyTracert::setDEF_MAX_HOP(int DEF_MAX_HOP)
{
    this->DEF_MAX_HOP = DEF_MAX_HOP;
}

USHORT MyTracert::checksum(USHORT *pBuf,int iSize)
{
    unsigned long cksum=0;
    while(iSize>1)
    {
        cksum+=*pBuf++;
        iSize-=sizeof(USHORT);
    }
    if(iSize)
    {
        cksum+=*(UCHAR *)pBuf;
    }
    cksum=(cksum>>16)+(cksum&0xffff);
    cksum+=(cksum>>16);
    return (USHORT)(~cksum);
}

//对数据包进行解码
bool MyTracert::DecodeIcmpResponse(char * pBuf,int iPacketSize,DECODE_RESULT &DecodeResult,BYTE ICMP_ECHO_REPLY,BYTE ICMP_TIMEOUT)
{
    //检查数据报大小的合法性
    IP_HEADER* pIpHdr = (IP_HEADER*)pBuf;
    int iIpHdrLen = pIpHdr->hdr_len * 4;
    if (iPacketSize < (int)(iIpHdrLen+sizeof(ICMP_HEADER)))
        return FALSE;
    //根据 ICMP 报文类型提取 ID 字段和序列号字段
    ICMP_HEADER *pIcmpHdr=(ICMP_HEADER *)(pBuf+iIpHdrLen);
    USHORT usID,usSquNo;
    if(pIcmpHdr->type==ICMP_ECHO_REPLY) //ICMP 回显应答报文
    {
        usID=pIcmpHdr->id; //报文 ID
        usSquNo=pIcmpHdr->seq; //报文序列号
    }
    else if(pIcmpHdr->type==ICMP_TIMEOUT)//ICMP 超时差错报文
    {
        char * pInnerIpHdr=pBuf+iIpHdrLen+sizeof(ICMP_HEADER); //载荷中的 IP 头
        int iInnerIPHdrLen=((IP_HEADER *)pInnerIpHdr)->hdr_len*4; //载荷中的 IP 头长
        ICMP_HEADER * pInnerIcmpHdr=(ICMP_HEADER *)(pInnerIpHdr+iInnerIPHdrLen);//载荷中的 ICMP 头
        usID=pInnerIcmpHdr->id; //报文 ID
        usSquNo=pInnerIcmpHdr->seq; //序列号
    }
    else
    {
        return false;
    }
    //检查 ID 和序列号以确定收到期待数据报
    if(usID!=(USHORT)GetCurrentProcessId()||usSquNo!=DecodeResult.usSeqNo)
    {
        return false;
    }
    //记录 IP 地址并计算往返时间
    DecodeResult.dwIPaddr.s_addr=pIpHdr->sourceIP;
    DecodeResult.dwRoundTripTime=GetTickCount()-DecodeResult.dwRoundTripTime;
    //处理正确收到的 ICMP 数据报
    if (pIcmpHdr->type == ICMP_ECHO_REPLY ||pIcmpHdr->type == ICMP_TIMEOUT)
    {
        //输出往返时间信息
        if(DecodeResult.dwRoundTripTime)
        {
            time = QString::number(DecodeResult.dwRoundTripTime);
            time += "ms";
            //            cout<<" "<<DecodeResult.dwRoundTripTime<<"ms"<<flush;

        }
        else
        {
            time = "<1ms";
            //            cout<<" "<<"<1ms"<<flush;
        }

    }
    return true;
}

bool MyTracert::isLegalIP(QString ip)
{
    WSADATA* wsa =(WSADATA*) malloc(sizeof(WSADATA));
    WSAStartup(MAKEWORD(2,2),wsa);
    bool flag = true;
    //得到 IP 地址
    u_long ulDestIP=inet_addr(ip.toUtf8().data());
    //转换不成功时按域名解析
    if(ulDestIP==INADDR_NONE)
    {
        hostent * pHostent=gethostbyname(ip.toUtf8().data());
        if(!pHostent)
        {
            flag = false;
        }
    }
    free(wsa);

    return flag;
}

void MyTracert::stopThread()
{
    this->threadStop = true;
}

//多线程
void MyTracert::run()
{
    threadStop = false;

    //初始化 Windows sockets 网络环境
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
    char IpAddress[255] ;
    strcpy(IpAddress,ip.toUtf8().data());

    //得到 IP 地址
    u_long ulDestIP=inet_addr(IpAddress);
    //转换不成功时按域名解析
    if(ulDestIP==INADDR_NONE)
    {
        hostent * pHostent=gethostbyname(IpAddress);
        if(pHostent)
        {
            ulDestIP=(*(in_addr*)pHostent->h_addr).s_addr;
        }
        else
        {
            WSACleanup();
            return ;
        }
    }

    //填充目地端 socket 地址
    sockaddr_in destSockAddr;
    ZeroMemory(&destSockAddr,sizeof(sockaddr_in));
    destSockAddr.sin_family=AF_INET; //地址族 使用 IPv4 进行通信
    destSockAddr.sin_addr.s_addr=ulDestIP; // 32位的IP地址
    //创建原始套接字 详细说明：https://www.cnblogs.com/hgwang/p/6118634.html
    SOCKET sockRaw=WSASocket(AF_INET,SOCK_RAW,IPPROTO_ICMP,NULL,0,
                             WSA_FLAG_OVERLAPPED); //SOCK_RAW表示原始套接字，即不是TCP也不是UDP

    //接收超时 (用于任意类型、任意状态套接口的设置选项值)
    setsockopt(sockRaw,SOL_SOCKET,SO_RCVTIMEO,(char *)&iTimeout,sizeof(iTimeout));
    //发送超时
    setsockopt(sockRaw,SOL_SOCKET,SO_SNDTIMEO,(char *)&iTimeout,sizeof(iTimeout));
    //构造 ICMP 回显请求消息，并以 TTL 递增的顺序发送报文
    //ICMP 类型字段
    const BYTE ICMP_ECHO_REQUEST=8; //请求回显
    const BYTE ICMP_ECHO_REPLY=0; //回显应答
    const BYTE ICMP_TIMEOUT=11; //传输超时
    //其他常量定义
    const int DEF_ICMP_DATA_SIZE=32; //ICMP 报文默认数据字段长度
    const int MAX_ICMP_PACKET_SIZE=1024;//ICMP 报文最大长度（包括报头）

    //填充 ICMP 报文中每次发送时不变的字段
    char IcmpSendBuf[sizeof(ICMP_HEADER)+DEF_ICMP_DATA_SIZE];//发送缓冲区
    memset(IcmpSendBuf, 0, sizeof(IcmpSendBuf)); //初始化发送缓冲区
    char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE]; //接收缓冲区
    memset(IcmpRecvBuf, 0, sizeof(IcmpRecvBuf)); //初始化接收缓冲区
    ICMP_HEADER * pIcmpHeader=(ICMP_HEADER*)IcmpSendBuf;
    pIcmpHeader->type=ICMP_ECHO_REQUEST; //类型为请求回显
    pIcmpHeader->code=0; //代码字段为 0
    pIcmpHeader->id=(USHORT)GetCurrentProcessId(); //ID 字段为当前进程号
    memset(IcmpSendBuf+sizeof(ICMP_HEADER),'E',DEF_ICMP_DATA_SIZE);//数据字段
    USHORT usSeqNo=0; //ICMP 报文序列号
    int iTTL=1; //TTL 初始值为 1
    BOOL bReachDestHost=FALSE; //循环退出标志
    int iMaxHot=DEF_MAX_HOP; //循环的最大次数
    DECODE_RESULT DecodeResult; //传递给报文解码函数的结构化参数
    while(!threadStop&&!bReachDestHost&&iMaxHot--)
    {
        //设置 IP 报头的 TTL 字段
        setsockopt(sockRaw,IPPROTO_IP,IP_TTL,(char *)&iTTL,sizeof(iTTL));
//        cout<<iTTL<<flush; //输出当前序号
        //填充 ICMP 报文中每次发送变化的字段
        ((ICMP_HEADER *)IcmpSendBuf)->cksum=0; //校验和先置为 0
        ((ICMP_HEADER *)IcmpSendBuf)->seq=htons(usSeqNo++); //填充序列号
        ((ICMP_HEADER *)IcmpSendBuf)->cksum=checksum((USHORT *)IcmpSendBuf,
                                                     sizeof(ICMP_HEADER)+DEF_ICMP_DATA_SIZE); //计算校验和
        //记录序列号和当前时间
        DecodeResult.usSeqNo=((ICMP_HEADER*)IcmpSendBuf)->seq; //当前序号
        DecodeResult.dwRoundTripTime=GetTickCount(); //当前时间
        //发送 TCP 回显请求信息
        sendto(sockRaw,IcmpSendBuf,sizeof(IcmpSendBuf),0,(sockaddr*)&destSockAddr,sizeof(destSockAddr));
        //接收 ICMP 差错报文并进行解析处理
        sockaddr_in from; //对端 socket 地址
        int iFromLen=sizeof(from); //地址结构大小
        int iReadDataLen; //接收数据长度
        while(!threadStop)
        {
            //接收数据
            iReadDataLen=recvfrom(sockRaw,IcmpRecvBuf,MAX_ICMP_PACKET_SIZE,0,(sockaddr*)&from,&iFromLen);
            if(iReadDataLen!=SOCKET_ERROR)//有数据到达
            {
                //对数据包进行解码
                if(DecodeIcmpResponse(IcmpRecvBuf,iReadDataLen,DecodeResult,ICMP_ECHO_REPLY,ICMP_TIMEOUT))
                {
                    //到达目的地，退出循环
                    if(DecodeResult.dwIPaddr.s_addr==destSockAddr.sin_addr.s_addr)
                        bReachDestHost=true;
                    //输出 IP 地址
                    emit this->sendResult(time + "#"+ QString(inet_ntoa(DecodeResult.dwIPaddr)));
//                    cout<<'\t'<<inet_ntoa(DecodeResult.dwIPaddr)<<endl;
                    break;
                }
            }
            else if(WSAGetLastError()==WSAETIMEDOUT) //接收超时，输出*号
            {
                emit this->sendResult("*#"+QString("Request timed out"));
//                cout<<" *"<<'\t'<<"Request timed out"<<endl;
                break;
            }
            else
            {
                break;
            }
        }
        iTTL++; //递增 TTL 值
    }

}
