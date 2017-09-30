#ifndef P2PUDPCHANNEL_H
#define P2PUDPCHANNEL_H
/*
 * p2p 连接过程(a-->s-->b)
 * 一、获取远端和本地端UDP地址
 * 1 UDP_P2P_ASK a向服务器s发送UDP_P2P_ASK请求，请求向b（dstUsrNu）进行p2p通信，a要带上自己本地端的UDP IP和端口。
 * 2 UDP_P2P_ASK s向b发送UDP_P2P_ASK请求，并带上a远端UDP地址和端口，告诉a想向其进行通信
 * 3 UDP_P2P_ASK b向s发送UDP_P2P_ASK回应，并带上b的本地UDP地址和端口，并设置Ack确认
 * 4 UDP_P2P_ASK s向a转发来自b的UDP_P2P_ASK回应，并带上b的远端UDP地址和端口，并设置Ack确认
 *
 * 二、P2P隧道打洞过程
 * 5 UDP_P2P_CONNECT1 a(b)进而向b(a)的本地地址B发送UDP_P2P_CONNECT1测试连接报文,如果能相互收到此报文，则相互回应ack标志位，p2p连接结束，然后进入第7步。
 * 6 UDP_P2P_CONNECT2 如果本地报文不通，则 a(b)进而向b(a)的外网地址发送UDP_P2P_CONNECT2测试连接报文，如果能相互收到此报文，则相互回应ack标志位
 * 7 UDP_P2P_KEEP     使p2p通道保持连接，每隔5秒发送一次，接到该报文后，把ack置1，然后把报文返回.
 * 8 UDP_P2P_DATA     a与b进行P2P的用户数据
*/
#include <QTimer>
#include <QObject>

#include "clockthreadman.h"

class ClockThreadMan;

enum {
    UDP_P2P_NONE,
    UDP_P2P_READ,
    UDP_P2P_CONNECT1,
    UDP_P2P_CONNECT2,
    UDP_P2P_KEEP,
    UDP_P2P_DATA

};

class ImmesageChannel;
class ImmessageData;

class P2PUdpChannel : public QObject
{    
public:
    typedef struct {
        unsigned int ipv4;
        unsigned short port;
    }netAdddr_t;

    typedef struct {
        netAdddr_t peerNetAddr[2];
        char addrIndex;
        char state;
        char tryTimes[2];
        int srcuid;
        int dstuid;
    }p2pChannelState_t;

    typedef struct {
        char state;
        //unsigned char srcChid;
        //unsigned char dstChid;
        char ack;
    }p2pTransData_t;

    P2PUdpChannel(QObject *parent);
    ~P2PUdpChannel();
    int getChannelId (netAdddr_t &loc, netAdddr_t &rem, int dstuid, int srcuid);
    int recvData (const ImmessageData &m);
    int sendData (const void *p, int len, int chId);
private:
    p2pChannelState_t p2pChan;
    ImmesageChannel *mesgChannelPtr;
    ClockThreadMan *clockThreadManPtr;
    ClockThreadMan::clocker_list p2pTimer;

    static void clockTaskTryP2p (void *p)
    {
        ((P2PUdpChannel*)p)->tryGetThroughP2pChan();
    }

    void initAllp2pChan ();
    int  parseP2PData (const ImmessageData &m);
    void setCurp2pState (char state, char addrIndex);
    int  sendData2NetWork (const p2pTransData_t *p2p, void *p = 0, int len = 0);

    void tryGetThroughP2pChan ();
};

#endif // P2PUDPCHANNEL_H
