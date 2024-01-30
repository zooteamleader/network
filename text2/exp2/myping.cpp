#include "myping.h"
#include <QDebug>

MyPing::MyPing(QObject *parent = nullptr,QStringList ipList = QStringList(""),int timeout = 1000,int count = 10) : QThread(parent)
{
    this->timeout = timeout;
    this->ipList = ipList;
    this->pingCount = count;
}


/*
解码 IP 标头找到ICMP 数据
*/
bool MyPing::decode_resp(char *buf, int bytes,struct sockaddr_in *from)//解析接收到的 ICMP 数据包，判断是否为期望的回复包，并输出相关信息
{
    //64 bytes from 112.80.248.76: icmp_seq = 4  time: 62 ms  ttl: 112
    IpHeader *iphdr;
    IcmpHeader *icmphdr;
    unsigned short iphdrlen;

    iphdr = (IpHeader *)buf;      //接收到的数据就是原始的IP数据报

    iphdrlen = iphdr->h_len * 4 ; // number of 32-bit words *4 = bytes

    if (bytes < iphdrlen + ICMP_MIN)
    {
        //        printf("Too few bytes from %s\n",inet_ntoa(from->sin_addr));
        return false;
    }

    icmphdr = (IcmpHeader*)(buf + iphdrlen);

    if(icmphdr->i_type == 3)
    {
        //        printf("network unreachable -- Response from %s.\n",inet_ntoa(from->sin_addr));
        return false;
    }

    if (icmphdr->i_id != (USHORT)GetCurrentProcessId())
    {
//        fprintf(stderr,"someone else's packet!\n");
        return false;
    }

    return true;

}

//完成ICMP的校验
USHORT MyPing::checksum(USHORT *buffer, int size)//计算 ICMP 包的校验和
{
    unsigned long cksum=0;

    while(size >1)
    {
        cksum+=*buffer++;
        size -=sizeof(USHORT);
    }

    if(size )
    {
        cksum += *(UCHAR*)buffer;
    }

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (USHORT)(~cksum);
}

/*
Helper 函数来填写 ICMP 请求中的各种内容。
*/
void MyPing::fill_icmp_data(char * icmp_data, int datasize)//填充 ICMP 数据包
{

    IcmpHeader *icmp_hdr;
    char *datapart;

    icmp_hdr = (IcmpHeader*)icmp_data;

    icmp_hdr->i_type = ICMP_ECHO;                   //ICMP_ECHO要求收到包的主机回复此ICMP包
    icmp_hdr->i_code = 0;
    icmp_hdr->i_id = (USHORT)GetCurrentProcessId(); //id填当前进程的id
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_seq = 0;

    datapart = icmp_data + sizeof(IcmpHeader);
    //
    // Place some junk in the buffer.
    //
    memset(datapart,'E', datasize - sizeof(IcmpHeader));  //填充了一些废物
}

bool MyPing::isLegalIP(QString ip)//检查 IP 地址是否合法
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

void MyPing::setIpList(QStringList ipList)//设置需要 ping 的 IP 列表
{
    this->ipList = ipList;
}

void MyPing::setTimeout(int timeout)//设置超时时间
{
    this->timeout = timeout;
}

void MyPing::setCount(int count)//设置 ping 包的数量
{
    this->pingCount = count;
}

void MyPing::stopThread()//停止线程
{
    this->threadStop = true;
}

void MyPing::run()//线程执行函数，依次 ping 列表中的 IP 地址，并将结果发送给主线程
{
    threadStop = false;

    WSADATA wsaData;
    SOCKET sockRaw;
    struct sockaddr_in dest;
    int bread,datasize;
    char *icmp_data;
    char *recvbuf;
    unsigned int addr=0;
    USHORT seq_no = 0;
    struct sockaddr_in from;
    int fromlen = sizeof(from);

    if (WSAStartup(MAKEWORD(2,1),&wsaData) != 0)
    {
        //        fprintf(stderr,"WSAStartup failed: %d\n",GetLastError());
        //        ExitProcess(STATUS_FAILED);
        emit this->sendError(QString("WSAStartup failed: \n%1").arg(GetLastError()));
        threadStop = true;
        return;
    }

    /*
    为了使用发送接收超时设置(即设置SO_RCVTIMEO, SO_SNDTIMEO)，
    //    必须将标志位设为WSA_FLAG_OVERLAPPED !
    */
    sockRaw = WSASocket (AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,WSA_FLAG_OVERLAPPED);                    //建立一个原始套接字
    //sockRaw = WSASocket (AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,0);

    if (sockRaw == INVALID_SOCKET)
    {
        //        fprintf(stderr,"WSASocket() failed: %d\n",WSAGetLastError());
        //        ExitProcess(STATUS_FAILED);
        emit this->sendError(QString("WSASocket() failed: \n%1").arg(WSAGetLastError()));
        threadStop = true;
        return;
    }

    bread = setsockopt(sockRaw,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout, sizeof(timeout)); //RECVTIMEO是接收超时时间
    if(bread == SOCKET_ERROR)
    {
        //        fprintf(stderr,"failed to set recv timeout: %d\n",WSAGetLastError());
        //        ExitProcess(STATUS_FAILED);
        emit this->sendError(QString("failed to set recv timeout: \n%1").arg(WSAGetLastError()));
        threadStop = true;
        return;
    }

    bread = setsockopt(sockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)); //SNDTIMEO是发送超时时间
    if(bread == SOCKET_ERROR)
    {
        //        fprintf(stderr,"failed to set send timeout: %d\n",WSAGetLastError());
        //        ExitProcess(STATUS_FAILED);
        emit this->sendError(QString("failed to set send timeout: \n%1").arg(WSAGetLastError()));
        threadStop = true;
        return;
    }

    bool successFlag = false;

    //设置目标ip
    for(int i=0;i<ipList.size() && (!threadStop);i++)
    {
        successFlag = false;
        emit this->sendResult(ipList[i] +"#"+"*");  //告诉界面正在ping

        memset(&dest,0,sizeof(dest));            //目标地址清零
        addr = inet_addr(ipList[i].toUtf8().data());    //第i个ip地址

        if (addr == INADDR_NONE)        //ip转化失败
        {
//            ExitProcess(STATUS_FAILED);
            successFlag = false;
            emit this->sendResult(ipList[i] +"#"+"NO");
            continue;
        }

        dest.sin_addr.s_addr = addr;
        dest.sin_family = AF_INET;
//        dest_ip = inet_ntoa(dest.sin_addr);        //目标IP地址
        datasize = DEF_PACKET_SIZE;             //ICMP包数据大小设定为32
        datasize += sizeof(IcmpHeader);         //另外加上ICMP包的包头 其实包头占12个字节
        icmp_data = (char *)malloc(MAX_PACKET);//发送icmp_data数据包内存
        recvbuf = (char *)malloc(MAX_PACKET);  //存放接收到的数据

        if (!icmp_data)                         //分配内存
        {
            emit this->sendError("分配内存失败");
            break;
        }
        memset(icmp_data,0,MAX_PACKET);
        fill_icmp_data(icmp_data,datasize);         //只填充了ICMP包
        int t = pingCount;
        while((t--) && (!threadStop))
        {
            int bwrote;

            ((IcmpHeader*)icmp_data)->i_cksum = 0;
            ((IcmpHeader*)icmp_data)->timestamp = GetTickCount(); //时间戳

            ((IcmpHeader*)icmp_data)->i_seq = seq_no++;           //ICMP的序列号
            ((IcmpHeader*)icmp_data)->i_cksum = checksum((USHORT*)icmp_data, datasize);   //icmp校验位

            //下面这个函数的问题是 发送数据只是ICMP数据包，而接收到的数据时包含ip头的 也就是发送和接收不对等
            //问题是sockRaw 设定了协议为 IPPROTO_ICMP
            bwrote = sendto(sockRaw,icmp_data,datasize,0,(struct sockaddr*)&dest, sizeof(dest));
            if (bwrote == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSAETIMEDOUT)     //发送时间超时
                {
//                    printf("timed out\n");
                    continue;
                }

//                fprintf(stderr,"sendto failed: %d\n",WSAGetLastError());
//                ExitProcess(STATUS_FAILED);
                emit this->sendError(QString("sendto failed: \n%1").arg(WSAGetLastError()));
                threadStop = true;
            }

            if (bwrote < datasize )
            {
//                fprintf(stdout,"Wrote %d bytes\n",bwrote);
            }

            bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,0,(struct sockaddr*)&from, &fromlen);
            if (bread == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSAETIMEDOUT)
                {
//                    printf("timed out\n");
                    continue;
                }
//                fprintf(stderr,"recvfrom failed: %d\n",WSAGetLastError());
//                ExitProcess(STATUS_FAILED);
                emit this->sendError(QString("recvfrom failed: \n%1").arg(WSAGetLastError()));
                threadStop = true;
            }
            successFlag = decode_resp(recvbuf,bread,&from);
            if(successFlag)
            {
                emit this->sendResult(ipList[i] +"#"+"在线");
                break;
            }
        }

        free(icmp_data);
        free(recvbuf);

        if(!successFlag)
        {
           emit this->sendResult(ipList[i] +"#"+"超时");
        }

    }

    WSACleanup();
}
