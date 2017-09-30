#include <string.h>
#include <QHostAddress>
#include <QDebug>
#include "p2pudpchannel.h"
#include "immessagchannel.h"


P2PUdpChannel::P2PUdpChannel(QObject *parent)
{
    clockThreadManPtr = ClockThreadMan::getInstance();
    mesgChannelPtr = ImmesageChannel::getInstance();
    initAllp2pChan();


}

P2PUdpChannel::~P2PUdpChannel()
{
    clockThreadManPtr->close();
    mesgChannelPtr->close();
}

int P2PUdpChannel::getChannelId(netAdddr_t &loc, netAdddr_t &rem, int dstuid, int srcuid)
{
    p2pChan.peerNetAddr[0] = loc;
    p2pChan.peerNetAddr[1] = rem;
    p2pChan.addrIndex = 0;
    p2pChan.state = UDP_P2P_READ;
    p2pChan.tryTimes[0] = -1;
    p2pChan.tryTimes[1] = 6;
    p2pChan.dstuid = dstuid;
    p2pChan.srcuid = srcuid;

    ClockThreadMan::ClockerInit(&p2pTimer, 100, clockTaskTryP2p, this);
    return clockThreadManPtr->addClocker(&p2pTimer);
}

void P2PUdpChannel::initAllp2pChan()
{
    memset(&p2pChan, 0, sizeof(p2pChan));
    p2pChan.state = UDP_P2P_NONE;

}

int P2PUdpChannel::recvData(const ImmessageData &m)
{
    return parseP2PData(m);
}

int P2PUdpChannel::parseP2PData(const ImmessageData &m)
{
    const p2pTransData_t *p2pdataPtr = (const p2pTransData_t*)m.getMesgDataPtr();
    p2pTransData_t p2pData;
    const int tmsc = 5000;
    return 0;
qDebug()<<"p2p udp recv cmd = "<<(int)p2pdataPtr->state<<" ack = "<<(int)p2pdataPtr->ack<<"state = "<<(int)p2pdataPtr->state<<" "<<m.getSrcUsrId()<<"-->"<<m.getDstUsrId();
    if (p2pdataPtr->state == UDP_P2P_CONNECT1){
        if (p2pdataPtr->ack){
            setCurp2pState(UDP_P2P_KEEP, 0);
            clockThreadManPtr->modClocker(&p2pTimer, tmsc);
        }else{
            p2pData.state = UDP_P2P_CONNECT1;
            p2pData.ack = 1;
            sendData2NetWork(&p2pData);
        }
    }else if (p2pdataPtr->state == UDP_P2P_CONNECT2){
        if (p2pdataPtr->ack){
            setCurp2pState(UDP_P2P_KEEP, 1);
            clockThreadManPtr->modClocker(&p2pTimer, tmsc);
        }else{
            p2pData.state = UDP_P2P_CONNECT2;
            p2pData.ack = 1;
            sendData2NetWork(&p2pData);
        }
    }else if (p2pdataPtr->state == UDP_P2P_KEEP){
        if (!p2pdataPtr->ack){
            p2pData.state = UDP_P2P_KEEP;
            p2pData.ack = 1;
            sendData2NetWork(&p2pData);
        }
    }else if (p2pdataPtr->state == UDP_P2P_DATA){
        return 1;
    }
    return 0;
}

void P2PUdpChannel::setCurp2pState(char state, char addrIndex)
{return;
    p2pChan.addrIndex = addrIndex;
    p2pChan.state = state;
}

int P2PUdpChannel::sendData2NetWork(const p2pTransData_t *p2p, void *p, int len)
{
    ImmessageData m(IMMESG_NETP2P_UDPDATA);

    m.setDstSrcUsr(p2pChan.dstuid, p2pChan.srcuid);
    m.addMesgData(sizeof(p2pTransData_t), p2p);
    if (p && len)
        m.addMesgData(len, p);

    QHostAddress dstAdrr(p2pChan.peerNetAddr[p2pChan.addrIndex].ipv4);
    qDebug()<<"send to"<<dstAdrr.toString();
    return mesgChannelPtr->pushUdpDataOut(m, dstAdrr.toString(), p2pChan.peerNetAddr[p2pChan.addrIndex].port);
}

void P2PUdpChannel::tryGetThroughP2pChan()
{
    p2pTransData_t p2pData;

    p2pData.ack = 0;
    if (p2pChan.state < UDP_P2P_KEEP){
        if (p2pChan.tryTimes[0] >= 0){
            p2pData.state = UDP_P2P_CONNECT1;
            --p2pChan.tryTimes[0];
            p2pChan.addrIndex = 0;
        }else if (p2pChan.tryTimes[1] >= 0){
            p2pData.state = UDP_P2P_CONNECT2;
            --p2pChan.tryTimes[1];
            p2pChan.addrIndex = 1;
        }else{
            qDebug()<<"udp p2p is not connnetc";
            //p2pTimer.stop();
            return;
        }
    }else{
        p2pData.state = UDP_P2P_KEEP;
    }
    sendData2NetWork(&p2pData);
}
