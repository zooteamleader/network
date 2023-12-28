#ifndef QUICKPING_H
#define QUICKPING_H

#include <QThread>
#include <winsock2.h>
#include <ws2tcpip.h>
#define WIN32_LEAN_AND_MEAN

#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

#define ICMP_MIN 8                                           // minimum 8 byte icmp packet (just header)

/* The IP header */
typedef struct iphdr
{
    unsigned int h_len:4; // length of the header
    unsigned int version:4; // Version of IP
    unsigned char tos; // Type of service
    unsigned short total_len; // total length of the packet
    unsigned short ident; // unique identifier
    unsigned short frag_and_flags; // flags
    unsigned char ttl;
    unsigned char proto; // protocol (TCP, UDP etc)
    unsigned short checksum; // IP checksum
    unsigned int sourceIP;
    unsigned int destIP;
} IpHeader;

//
// ICMP header
//
typedef struct _ihdr
{
    BYTE i_type;     //消息类型
    BYTE i_code;     //代码  /* type sub code */
    USHORT i_cksum;  //校验和
    USHORT i_id;     //ID号
    USHORT i_seq;    //序列号
    ULONG timestamp; //时间戳  /* This is not the std header, but we reserve space for time */
} IcmpHeader;        //ICMP报文  包括报头和数据

#define STATUS_FAILED 0xFFFF
#define DEF_PACKET_SIZE 32
#define MAX_PACKET 1024

#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(p) HeapFree (GetProcessHeap(),0,(p))


class MyPing : public QThread
{
    Q_OBJECT
public:
    explicit MyPing(QObject *parent,QStringList ipList,int timeout,int pingCount );
    void setTimeout(int timeout);   //设置发送超时时间
    void setIpList(QStringList ipList);
    void setCount(int pingCount);
    void run();

    static bool isLegalIP(QString ip);

    bool threadStop = false;

signals:
    void sendResult(QString data);
    void sendError(QString data);

public slots:
    void stopThread();

private:
    int timeout = 1000;   //设置发送超时时间
    QStringList ipList;
    int pingCount = 10;   //ping 次数

    void fill_icmp_data(char *, int);
    USHORT checksum(USHORT *, int);
    bool decode_resp(char *,int,struct sockaddr_in *);
};

#endif // QUICKPING_H
