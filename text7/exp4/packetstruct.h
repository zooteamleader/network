#ifndef PACKETSTRUCT_H
#define PACKETSTRUCT_H
#pragma pack(1)


/*以太网帧头格式结构体 14个字节*/
typedef struct ether_header
{
    unsigned char ether_dhost[6];// 目的MAC地址
    unsigned char ether_shost[6];// 源MAC地址
    unsigned short ether_type;// eh_type的值需要考察上一层的协议，如果为ip则为0×0800
}ETHERHEADER, *PETHERHEADER;

/*以ARP字段结构体 28个字节*/
typedef struct arp_header
{
    unsigned short arp_hrd;
    unsigned short arp_pro;
    unsigned char arp_hln;
    unsigned char arp_pln;
    unsigned short arp_op;
    unsigned char arp_sourha[6];
    unsigned long arp_sourpa;
    unsigned char arp_destha[6];
    unsigned long arp_destpa;
}ARPHEADER, *PARPHEADER;

/*ARP报文结构体 42个字节*/
typedef struct arp_packet
{
    ETHERHEADER etherHeader;
    ARPHEADER   arpHeader;
}ARPPACKET, *PARPPACKET;


// ipv4_pro字段：
#define PROTOCOL_ICMP   0x01
#define PROTOCOL_IGMP   0x02
#define PROTOCOL_TCP    0x06
#define PROTOCOL_UDP    0x11

/*IPv4报头结构体 20个字节*/
typedef struct ipv4_header
{
    unsigned char ipv4_ver_hl;// Version(4 bits) + Internet Header Length(4 bits)长度按4字节对齐
    unsigned char ipv4_stype; // 服务类型
    unsigned short ipv4_plen;// 总长度（包含IP数据头，TCP数据头以及数据）
    unsigned short ipv4_pidentify;// ID定义单独IP
    unsigned short ipv4_flag_offset;// 标志位偏移量
    unsigned char ipv4_ttl; // 生存时间
    unsigned char ipv4_pro;// 协议类型
    unsigned short ipv4_crc;// 校验和
    unsigned long  ipv4_sourpa;// 源IP地址
    unsigned long  ipv4_destpa;// 目的IP地址
}IPV4HEADER, *PIPV4HEADER;


/*IPv6报头结构体 40个字节*/
typedef struct ipv6_header
{
    unsigned char ipv6_ver_hl;
    unsigned char ipv6_priority;
    unsigned short ipv6_lable;
    unsigned short ipv6_plen;
    unsigned char  ipv6_nextheader;
    unsigned char  ipv6_limits;
    unsigned char ipv6_sourpa[16];
    unsigned char ipv6_destpa[16];
}IPV6HEADER, *PIPV6HEADER;

/*TCP报头结构体 20个字节*/
typedef struct tcp_header
{
    unsigned short tcp_sourport;//源端口
    unsigned short tcp_destport;//目的端口
    unsigned long  tcp_seqnu;//序列号
    unsigned long  tcp_acknu;//确认号
    unsigned char  tcp_hlen; //4位首部长度 还有4位保留
    unsigned char  tcp_reserved;//前2位保留 后6位标志位
    unsigned short tcp_window;//窗口大小
    unsigned short tcp_chksum;//检验和
    unsigned short tcp_urgpoint;//紧急指针
}TCPHEADER, *PTCPHEADER;

/*UDP报头结构体 8个字节*/
typedef struct udp_header
{
    unsigned short udp_sourport;// 源端口
    unsigned short udp_destport;// 目的端口
    unsigned short udp_hlen;// 长度
    unsigned short udp_crc;// 校验和
}UDPHEADER, *PUDPHEADER;


#pragma pack()






#endif // PACKETSTRUCT_H
