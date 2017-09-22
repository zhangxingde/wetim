#include "clienudpevent.h"
#include "immesgdecor.h"
#include "stdebug.h"
#include "usronlinestate.h"

ClientUdpImmesgEvent::ClientUdpImmesgEvent()
{
    LLIST_INIT(&inDataQueue);
    LLIST_INIT(&outDataQueue);
    mdbptr = SevSqlDB::getInstance();
}

ClientUdpImmesgEvent::~ClientUdpImmesgEvent()
{
    mdbptr->close();
}

void ClientUdpImmesgEvent::close()
{
    if (!LLIST_EMPTY(&outDataQueue)){
        ll_list_t *l = outDataQueue.next;

        LLIST_DEL(&outDataQueue);
        mempollPtr->freeMemList(l);
    }
    STD_DEBUG("udp close");
}

int ClientUdpImmesgEvent::doSelfWorkByRecvData(ll_list_t *head)
{
    udpDataIterm_t *u;
    memBbuff_t *p;
    Immessage_t *m;

    p = MEMBER_ENTRY(head, memBbuff_t, list);
    u = (udpDataIterm_t*)p->s;
    m = (Immessage_t*)u->dataChar;

    if (u->dataLen != (ntohl(m->mesgLen) + sizeof(*m))){
        mempollPtr->freeMemList(head);
        return 0;
    }
    LLIST_ADDLINK(&outDataQueue, head);

    return 1;
}

int ClientUdpImmesgEvent::doSelfWorkBySendData(ll_list_t *head)
{
    uint32_t mesgDataLen = 0, shouldDataLen = 0, shouldMoreLen = 0, moreLen = 0;
    memBbuff_t *b;
    Immessage_t *m;
    udpDataIterm_t *u;
    ImmessageData messageData;
    int retnum = 0;

    while (!LLIST_EMPTY(&outDataQueue)){
        b = MEMBER_ENTRY(outDataQueue.next, memBbuff_t, list);
        u = (udpDataIterm_t*)b->s;
        b->dataBeginPtr = u->dataChar;
        m = (Immessage_t*)u->dataChar;
        if (!mesgDataLen){
            shouldDataLen = ntohl(m->mesgLen) + sizeof(Immessage_t);
        }
        shouldMoreLen = shouldDataLen - mesgDataLen;
        moreLen = b->dataEndPtr - b->dataBeginPtr;
        if (shouldMoreLen < moreLen){
            messageData.recvRawData(b->dataBeginPtr, shouldMoreLen);
            b->dataBeginPtr += shouldMoreLen;
            mesgDataLen += shouldMoreLen;
        }else{
            messageData.recvRawData(b->dataBeginPtr, moreLen);
            mesgDataLen += moreLen;
            LLIST_DEL(&b->list);
            mempollPtr->freeMembuf(b);
        }
        if (mesgDataLen == shouldDataLen){
            retnum = 1;
            break;
        }

    }

    if (!retnum || !messageData.isValid()){
        return 0;
    }
    retnum = doSevByMesgType(u->inAddr, messageData, head);
    STD_DEBUG(" udp out ret = %d",retnum);
    return retnum;
}

int ClientUdpImmesgEvent::doSevByMesgType(struct sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out)
{
    switch (imsgData.getMesgType()){
        case IMMESG_UDP_KEEPALIVE:
            return replyCmdKeepAlive(srcaddr, imsgData, out);
        case IMMESG_USER_BROAD:
            return replyCmdBroadWentOn(srcaddr, imsgData, out);

    }
    return 0;
}

void ClientUdpImmesgEvent::broadCurUsrWentOn(int uid)
{

}

int ClientUdpImmesgEvent::replyCmdKeepAlive(sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out)
{
    ImmessageData m(IMMESG_UDP_KEEPALIVE);
    memBbuff_t *b = 0;
    udpDataIterm_t dstUdpData;

    m.setDstSrcUsr(imsgData.getSrcUsrId(),0);
    dstUdpData.addrLen = sizeof(struct sockaddr_in);
    dstUdpData.inAddr = srcaddr;
    dstUdpData.dataLen = m.length();
    b = mempollPtr->getMembuf();
    if (!b)
        return 0;
    mempollPtr->memDataCopy(b, &dstUdpData, getUdpDataItermHeadLen());
    mempollPtr->memDataCopy(b, m.getDataPtr(), m.length());
    LLIST_ADD_TAIL(out, &b->list);
    //STD_DEBUG("%s:%d, %lu", inet_ntoa(srcaddr.sin_addr), ntohs(srcaddr.sin_port), b->dataEndPtr - b->dataBeginPtr);
    return 1;
}

int ClientUdpImmesgEvent::replyCmdBroadWentOn(sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out)
{
    memBbuff_t *b = 0;
    udpDataIterm_t dstUdpData;
    UsrOnlineState onState;
    const int maxn = 100;
    UsrOnlineState::UsrOnState_t onbuf[maxn];
    int n = 0, r;

    ImmessageData m(IMMESG_USER_ONLIST);
    ImmesgDecorOnlist on(&m);
    char srcName[32];

    mdbptr->getUsrNameByUsrID(imsgData.getSrcUsrId(), srcName, sizeof(srcName));
    on.setHadMore(0);
    on.addOneUsr(imsgData.getSrcUsrId(), srcName, mdbptr->getAvaiconIdByUsrID(imsgData.getSrcUsrId()));
    STD_DEBUG();
    do {//以后再完善对任意多的用户广播数据
        if ((r = onState.getOnlineUsrInfo(onbuf, maxn, n)) > 0){
            n += r;
            for (int i = 0; i < r; ++i){
                b = mempollPtr->getMembuf();
                if (!b)
                    return 1;
                on.setDstSrcUsr(onbuf[i].uid, imsgData.getSrcUsrId());

                memset(&dstUdpData, 0, sizeof(dstUdpData));
                dstUdpData.addrLen = sizeof(struct sockaddr_in);
                dstUdpData.inAddr.sin_addr.s_addr = onbuf[i].srcaddr;
                dstUdpData.inAddr.sin_port = onbuf[i].srcport;
                dstUdpData.dataLen = on.length();
                mempollPtr->memDataCopy(b, &dstUdpData, getUdpDataItermHeadLen());
                mempollPtr->memDataCopy(b, on.getDataPtr(), on.length());
                LLIST_ADD_TAIL(out, &b->list);
            }
        }
        STD_DEBUG("r = %d", r);
    }while (r == maxn);

    onbuf[0].uid = imsgData.getSrcUsrId();
    onbuf[0].srcaddr = srcaddr.sin_addr.s_addr;
    onbuf[0].srcport = srcaddr.sin_port;
    onState.addUser(onbuf[0]);
    STD_DEBUG();
    return 1;
}

