#ifndef MYPING_H
#define MYPING_H

#include <QThread>
#include <winsock2.h>
#include <ws2tcpip.h>

//IP 报头
typedef struct
{
    unsigned char hdr_len:4; //4 位头部长度
    unsigned char version:4; //4 位版本号
    unsigned char tos; //8 位服务类型
    unsigned short total_len; //16 位总长度
    unsigned short identifier; //16 位标识符
    unsigned short frag_and_flags; //3 位标志加 13 位片偏移
    unsigned char ttl; //8 位生存时间
    unsigned char protocol; //8 位上层协议号
    unsigned short checksum; //16 位校验和
    unsigned long sourceIP; //32 位源 IP 地址
    unsigned long destIP; //32 位目的 IP 地址
} IP_HEADER;

typedef struct
{
    BYTE type; //8 位类型字段
    BYTE code; //8 位代码字段
    USHORT cksum; //16 位校验和
    USHORT id; //16 位标识符
    USHORT seq; //16 位序列号
} ICMP_HEADER;

//报文解码结构
typedef struct
{
    USHORT usSeqNo; //序列号
    DWORD dwRoundTripTime; //往返时间
    in_addr dwIPaddr; //返回报文的 IP 地址
} DECODE_RESULT;


class MyTracert : public QThread
{
    Q_OBJECT
public:
    explicit MyTracert(QObject *parent,QString ip ,int iTimeout ,int DEF_MAX_HOP);

    void run();

    void setIP(QString ip);
    void setiTimeout(int iTimeout);
    void setDEF_MAX_HOP(int DEF_MAX_HOP);
    static bool isLegalIP(QString ip);

    bool threadStop = false;

private:
    QString ip; //设置ip
    int iTimeout=3000;  //超时时间
    int DEF_MAX_HOP=30; //最大跳站数

    QString time;

    USHORT checksum(USHORT *pBuf,int iSize);
    bool DecodeIcmpResponse(char * pBuf,int iPacketSize,DECODE_RESULT &DecodeResult,BYTE ICMP_ECHO_REPLY,BYTE ICMP_TIMEOUT);

signals:
    void sendResult(QString data);

public slots:
    void stopThread();

};

#endif // MYPING_H
