#ifndef P2PUDPCHANNEL_H
#define P2PUDPCHANNEL_H
/*
 * p2p 连接过程
 * 一、获取远端和本地端UDP地址
 * (a-->s-->b)
 * 1 UDP_P2P_REQUEST a向服务器s发送UDP_P2P_REQUEST请求，请求向b（dstUsrNu）进行p2p通信，a要带上自己的p2p通道ID和本地UDP地址和端口。
 * 2 UDP_P2P_REQUEST s向b转发送UDP_P2P_REQUEST请求，告诉a想向其进行通信。
 * 3 UDP_P2P_REQUEST b向s发送UDP_P2P_REQUEST回应，并带上b的本地UDP地址和端口，和自己的p2p通道ID，并设置Ack确认
 * 4 UDP_P2P_REQUEST s向a转发来自b的UDP_P2P_REQUEST回应，并带上b的远端UDP地址和端口，并设置Ack确认
 * 5 UDP_P2P_REQUEST a收到带Ack确认的回复报文时，设置对方的远端和本地端地址以及对方的p2p通道ID，然后进入P2P隧道打洞过程
 *
 * (b-->s-->a)
 *
 * 1 在(a-->s-->b)的第二步中，收到来自a的p2p请求，设置对方的远端和本地端地址以及对方的p2p通道ID，然后进入P2P隧道打洞过程
 *
 * 二、P2P隧道打洞过程
 * 5 UDP_P2P_CONNECT1 a(b)进而向b(a)的本地地址B发送UDP_P2P_CONNECT1测试连接报文,如果能相互收到此报文，则相互回应ack标志位，p2p连接结束，然后进入第7步。
 * 6 UDP_P2P_CONNECT2 如果本地报文不通，则 a(b)进而向b(a)的外网地址发送UDP_P2P_CONNECT2测试连接报文，如果能相互收到此报文，则相互回应ack标志位
 * 7 UDP_P2P_KEEP     使p2p通道保持连接，每隔5秒发送一次，接到该报文后，把ack置1，然后把报文返回.
*/

#include "immesgdecor.h"
#include "clockthreadman.h"
#include "gccatomic.h"

class ClockThreadMan;
class ImmesageChannel;


typedef void (*p2pChannelNoticeCallbak)(int chid, netAddr_t dstAddr, int type, void *p);

class P2PUdpChannel : public QObject
{    
public:
#define UDP_P2P_NONE     ImmesgDecorP2PUdpData::UDP_P2P_NONE
#define UDP_P2P_REQUEST  ImmesgDecorP2PUdpData::UDP_P2P_REQUEST
#define UDP_P2P_CONNECT1 ImmesgDecorP2PUdpData::UDP_P2P_CONNECT1
#define UDP_P2P_CONNECT2 ImmesgDecorP2PUdpData::UDP_P2P_CONNECT2
#define UDP_P2P_KEEP     ImmesgDecorP2PUdpData::UDP_P2P_KEEP
#define UDP_P2P_CLOSE     ImmesgDecorP2PUdpData::UDP_P2P_CLOSE
    #pragma pack(1)
    typedef struct {
        P2PUdpChannel *p;
        int chid;
    }TimerFunArg_t;
    typedef struct {
        netAddr_t peerNetAddr[2];
        char addrIndex;
        QAtomicInt state;
        char tryTimes[2];
        int srcuid;
        int dstuid;
        int srcChid;
        int dstChid;
        int isInited;
        p2pChannelNoticeCallbak f;
        void *callbakArg;
        ClockThreadMan::clocker_list p2pTimer;
        TimerFunArg_t funarg;
    }p2pChannelState_t;
    #pragma pack()

    P2PUdpChannel(QObject *parent);
    ~P2PUdpChannel();

    int getNewChannelId (int dstuid, int srcuid);
    int findChannelId (int dstuid, int srcuid);
    void closeChannle (int chid, int isFrist = 1);
    void setChannelNoticeCallbak (int chid, p2pChannelNoticeCallbak f, void *p)
    {
        p2pChans[chid].f = f;
        p2pChans[chid].callbakArg = p;
    }
    netAddr_t getP2PDstPeerAddr (int chid) {return p2pChans[chid].peerNetAddr[p2pChans[chid].addrIndex];}
    int startGetThroughP2P (int srcChid, int dstChid = -1, int isInited = 1);


    //int getChannelId (int dstuid, int srcuid, int isInited = 1);

    int recvData (const ImmessageData &m);

private:
    p2pChannelState_t p2pChans[128];
    const int maxChanNum;
    ImmesageChannel *mesgChannelPtr;
    ClockThreadMan *clockThreadManPtr;

    static void clockTaskTryP2p (void *p)
    {
        TimerFunArg_t *arg = (TimerFunArg_t*)p;

        if (arg){
            arg->p->tryGetThroughP2pChan(arg->chid);
        }
    }

    void initAllp2pChan ();
    int sendP2pRequest2DstUsr (p2pChannelState_t &p2pChan, int isInited);
    int  parseP2PData (const ImmessageData &m);
    void setCurp2pState (p2pChannelState_t &p2pChan,char state, char addrIndex);
    int  replyP2PCmd2SrcUsr (const ImmesgDecorP2PUdpData &srcdata, char curState);
    int  sendP2PData2DstChannle (const p2pChannelState_t &p2pChan, char state, bool ack);
    void tryGetThroughP2pChan (int chid);
};

#endif // P2PUDPCHANNEL_H
