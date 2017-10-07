#include <string.h>
#include <QHostAddress>
#include <QDebug>
#include "p2pudpchannel.h"
#include "immessagchannel.h"
#include "climesgobsev.h"

P2PUdpChannel::P2PUdpChannel(QObject *parent):maxChanNum(sizeof(p2pChans)/sizeof(p2pChans[0]))
{
    clockThreadManPtr = ClockThreadMan::getInstance();
    mesgChannelPtr = ImmesageChannel::getInstance();
    initAllp2pChan();

    mesgChannelPtr->regOneImobsever(new ImesgP2pUdp, this);
}

P2PUdpChannel::~P2PUdpChannel()
{
    clockThreadManPtr->close();
    mesgChannelPtr->close();
}

int P2PUdpChannel::getNewChannelId(int dstuid, int srcuid)
{
    int i = 0;
    for (i = 0; i < maxChanNum; ++i){
        if (ATOMIC_BOOL_CMP_SWAP(&p2pChans[i].state, UDP_P2P_NONE, UDP_P2P_REQUEST)){
            break;
        }
    }
    if (i >= maxChanNum)
        return -1;
    p2pChannelState_t &p2pChan = p2pChans[i];

    p2pChan.addrIndex = -1;
    p2pChan.state = UDP_P2P_REQUEST;
    p2pChan.tryTimes[0] = 3;
    p2pChan.tryTimes[1] = 6;
    p2pChan.dstuid = dstuid;
    p2pChan.srcuid = srcuid;
    p2pChan.srcChid = i;
    ClockThreadMan::ClockerInit(&p2pChan.p2pTimer,clockTaskTryP2p, &p2pChan.funarg);
    return i;
}

int P2PUdpChannel::findChannelId(int dstuid, int srcuid)
{
    int i;

    for (i = 0; i < maxChanNum; ++i){
        if (p2pChans[i].dstuid == dstuid && p2pChans[i].srcuid == srcuid){
            return i;
        }
    }
    return -1;
}

void P2PUdpChannel::closeChannle(int chid, int isFrist)
{
    p2pChannelState_t &p2pChan = p2pChans[chid];

    clockThreadManPtr->delCloker(&p2pChan.p2pTimer);
    ATOMIC_GET_SET(&p2pChan.state, UDP_P2P_NONE);

    if (isFrist)
        sendP2PData2DstChannle(p2pChan, UDP_P2P_CLOSE, 0);
    memset(&p2pChan, 0, sizeof(p2pChan));
}

int P2PUdpChannel::startGetThroughP2P(int srcChid, int dstChid,int isInited)
{
    p2pChans[srcChid].isInited = isInited;
    p2pChans[srcChid].dstChid = dstChid;
    return sendP2pRequest2DstUsr(p2pChans[srcChid],isInited);
}

void P2PUdpChannel::initAllp2pChan()
{
    memset(&p2pChans, 0, sizeof(p2pChans));
    for (int i = 0; i < maxChanNum; ++i){
        p2pChans[i].state = UDP_P2P_NONE;
    }
}

int P2PUdpChannel::sendP2pRequest2DstUsr(p2pChannelState_t &p2pChan, int isInited)
{
    ImmessageData m(IMMESG_NETP2P_UDPDATA);
    ImmesgDecorP2PUdpData udp(&m);

    udp.setDstSrcUsr(p2pChan.dstuid, p2pChan.srcuid);
    udp.setSrcUsrLocUdpAddr(mesgChannelPtr->getLocalUdpIpv4(), mesgChannelPtr->getLocalUdpProt());
    udp.setAck(!isInited);
    udp.setState(p2pChan.state);
    udp.setSrcChid(p2pChan.srcChid);
    udp.setDstChid(p2pChan.dstChid);

    return mesgChannelPtr->pushUdpDataOut(m);
}

int P2PUdpChannel::recvData(const ImmessageData &m)
{
    return parseP2PData(m);
}

int P2PUdpChannel::parseP2PData(const ImmessageData &m)
{
    ImmesgDecorP2PUdpData p2pData(&m);
    const int tmsc = 5000;
    p2pChannelState_t *p2pChanPtr = 0;

qDebug()<<"chid = "<<p2pData.getSrcChid()<<" state = "<<(int)p2pData.getState()<<" ack = "<<p2pData.isAck()<<" "<<m.getSrcUsrId()<<"-->"<<m.getDstUsrId();
    if (p2pData.getState() == UDP_P2P_REQUEST){
        int chid = -1;
        if (!p2pData.isAck()){//来自对端的 UDP_P2P_REQUEST 请求P2P请求本地端口，将要进行回复
            chid = getNewChannelId(m.getSrcUsrId(), m.getDstUsrId());
            startGetThroughP2P(chid, p2pData.getSrcChid(),0);
            p2pChanPtr = p2pChans + chid;
            p2pChanPtr->funarg.chid = chid;
        }else{
            p2pChanPtr = p2pChans + p2pData.getDstChid();
            p2pChanPtr->dstChid = p2pData.getSrcChid();
            p2pChanPtr->funarg.chid = p2pData.getDstChid();
        }//然后开发p2p隧道打通
        p2pChanPtr->addrIndex = 0;
        p2pChanPtr->peerNetAddr[0] = p2pData.getSrcUsrLocUdpAddr();
        p2pChanPtr->peerNetAddr[1] = p2pData.getSrcUsrRemUdpAddr();
        setCurp2pState(*p2pChanPtr,UDP_P2P_CONNECT1, 0);
        p2pChanPtr->funarg.p = this;
        clockThreadManPtr->addClocker(&p2pChanPtr->p2pTimer, 200);
    }else if (p2pData.getState() == UDP_P2P_CONNECT1){
        p2pChannelState_t &p2pChan = p2pChans[p2pData.getDstChid()];
        if (p2pData.isAck()){
            setCurp2pState(p2pChan, UDP_P2P_KEEP, 0);
            if (!p2pChan.isInited)
                clockThreadManPtr->delCloker(&p2pChan.p2pTimer);
            else
                clockThreadManPtr->modClocker(&p2pChan.p2pTimer, tmsc);
            if (p2pChan.f){
                p2pChan.f(p2pChan.srcChid, p2pChan.peerNetAddr[p2pChan.addrIndex], 0,p2pChan.callbakArg);
            }
        }else{
            replyP2PCmd2SrcUsr(p2pData, UDP_P2P_CONNECT1);
        }
    }else if (p2pData.getState() == UDP_P2P_CONNECT2){
        p2pChannelState_t &p2pChan = p2pChans[p2pData.getDstChid()];
        if (p2pData.isAck()){
            setCurp2pState(p2pChan, UDP_P2P_KEEP, 1);
            if (!p2pChan.isInited)
                clockThreadManPtr->delCloker(&p2pChan.p2pTimer);
            else
                clockThreadManPtr->modClocker(&p2pChan.p2pTimer, tmsc);
            if (p2pChan.f){
                p2pChan.f(p2pChan.srcChid, p2pChan.peerNetAddr[p2pChan.addrIndex], 0,p2pChan.callbakArg);
            }
        }else{
            replyP2PCmd2SrcUsr(p2pData, UDP_P2P_CONNECT1);
        }
    }else if (p2pData.getState() == UDP_P2P_KEEP){
        if (!p2pData.isAck()){
            replyP2PCmd2SrcUsr(p2pData, UDP_P2P_KEEP);
        }
    }else if (p2pData.getState() == UDP_P2P_CLOSE){
        closeChannle(p2pData.getDstChid(), 0);
    }
    return 1;
}

void P2PUdpChannel::setCurp2pState(p2pChannelState_t &p2pChan, char state, char addrIndex)
{
    p2pChan.addrIndex = addrIndex;
    p2pChan.state = state;
}

int P2PUdpChannel::replyP2PCmd2SrcUsr(const ImmesgDecorP2PUdpData &srcdata, char curState)
{
    p2pChannelState_t &p2pChan = p2pChans[srcdata.getDstChid()];
    return sendP2PData2DstChannle(p2pChan, curState, 1);
}

int P2PUdpChannel::sendP2PData2DstChannle(const p2pChannelState_t &p2pChan, char state, bool ack)
{
    ImmessageData m(IMMESG_NETP2P_UDPDATA);
    ImmesgDecorP2PUdpData p2pData(&m);

    p2pData.setDstSrcUsr(p2pChan.dstuid,  p2pChan.srcuid);
    p2pData.setAck(ack);
    p2pData.setDstChid(p2pChan.dstChid);
    p2pData.setSrcChid(p2pChan.srcChid);
    p2pData.setState(state);

    QHostAddress dstAdrr(p2pChan.peerNetAddr[p2pChan.addrIndex].ipv4);
    qDebug()<<"send to"<<dstAdrr.toString()<<":"<<QString::number(p2pChan.peerNetAddr[p2pChan.addrIndex].port)<<" ack = "<<ack<<" dstchid = "<<p2pChan.dstChid;
    return mesgChannelPtr->pushUdpDataOut(m, dstAdrr.toString(), p2pChan.peerNetAddr[p2pChan.addrIndex].port);
}

void P2PUdpChannel::tryGetThroughP2pChan(int chid)
{
    p2pChannelState_t &p2pChan = p2pChans[chid];
    char state;
qDebug()<<"try send chid = "<<chid<<" dstchid = "<<p2pChan.dstChid;
    if (p2pChan.state < UDP_P2P_KEEP){
        if (p2pChan.tryTimes[0] >= 0){
            state = UDP_P2P_CONNECT1;
            --p2pChan.tryTimes[0];
            p2pChan.addrIndex = 0;
        }else if (p2pChan.tryTimes[1] >= 0){
            state = UDP_P2P_CONNECT2;
            --p2pChan.tryTimes[1];
            p2pChan.addrIndex = 1;
        }else{
            closeChannle(chid);
            qDebug()<<"udp p2p is not connnetc";
            return;
        }
    }else{
        state = UDP_P2P_KEEP;
    }
    sendP2PData2DstChannle(p2pChan, state, 0);
}
