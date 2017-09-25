#include "clienudpevent.h"
#include "immesgdecor.h"
#include "stdebug.h"
#include "usronlinestate.h"

ClientUdpImmesgEvent::ClientUdpImmesgEvent()
{
    LLIST_INIT(&inDataQueue);
    LLIST_INIT(&outDataQueue);
    mdbptr = SevSqlDB::getInstance();
    lastRemainTaskType = IMMESG_NONE;
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
    lastRemainTaskType = IMMESG_NONE;
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
    struct sockaddr_in srcaddr;
    ImmessageData messageData;
    int retnum = 0;
    static void *lastRemainTaskArgPtr = 0;

    if (doTheLastRemainTask(lastRemainTaskType, lastRemainTaskArgPtr, head)){
        return 1;
    }

    while (!LLIST_EMPTY(&outDataQueue)){
        b = MEMBER_ENTRY(outDataQueue.next, memBbuff_t, list);
        u = (udpDataIterm_t*)b->s;
        srcaddr = u->inAddr;
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
    retnum = doSevByMesgType(srcaddr, messageData, head, &lastRemainTaskArgPtr);
    return retnum;
}

int ClientUdpImmesgEvent::doSevByMesgType(struct sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p)
{
    switch (imsgData.getMesgType()){
        case IMMESG_UDP_KEEPALIVE:
            return replyCmdKeepAlive(srcaddr, imsgData, out, p);
        case IMMESG_USER_BROAD:
            return replyCmdBroadWentOn(srcaddr, imsgData, out, p);

    }
    return 0;
}

int ClientUdpImmesgEvent::replyCmdKeepAlive(sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p)
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
    return 1;
}

int ClientUdpImmesgEvent::replyCmdBroadWentOn(sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p)
{
    UsrOnlineState onState;
    UsrOnlineState::UsrOnState_t onbuf[1];
    static int wentOnUsrId = 0;
    int r;

    wentOnUsrId = imsgData.getSrcUsrId();
    r = loopBroadUsrWenton(&wentOnUsrId, out);

    onbuf[0].uid = imsgData.getSrcUsrId();
    onbuf[0].srcaddr = srcaddr.sin_addr.s_addr;
    onbuf[0].srcport = srcaddr.sin_port;
    onState.addUser(onbuf[0]);
    *p = &wentOnUsrId;
    return r;
}

int ClientUdpImmesgEvent::doTheLastRemainTask(int cmdtpe, void *p, ll_list_t *out)
{
    if (cmdtpe == IMMESG_NONE)
        return 0;
    else if (cmdtpe == IMMESG_USER_BROAD){
        return loopBroadUsrWenton(p, out);
    }
    return 0;
}

int ClientUdpImmesgEvent::loopBroadUsrWenton(void *p, ll_list_t *out)
{
    memBbuff_t *b = 0;
    udpDataIterm_t dstUdpData;
    UsrOnlineState onState;
    const int maxn = 100;
    UsrOnlineState::UsrOnState_t onbuf[maxn];
    int wentOnUsrId = *(int*)p;
    static ll_list_t *lastEndPtr = 0;
    int n = 0;

    ImmessageData m(IMMESG_USER_ONLIST);
    ImmesgDecorOnlist on(&m);
    char srcName[32];

    mdbptr->getUsrNameByUsrID(wentOnUsrId, srcName, sizeof(srcName));
    on.addOneUsr(wentOnUsrId, srcName, mdbptr->getAvaiconIdByUsrID(wentOnUsrId));
    n = onState.getOnlineUsrInfo(onbuf, maxn, &lastEndPtr);
    if (n > 0){
        for (int i = 0; i < n; ++i){
           b = mempollPtr->getMembuf();
           if (!b)
               return 1;//下次继续
           on.setDstSrcUsr(onbuf[i].uid, wentOnUsrId);
           memset(&dstUdpData, 0, sizeof(dstUdpData));
           dstUdpData.addrLen = sizeof(struct sockaddr_in);
           dstUdpData.inAddr.sin_family = AF_INET;
           dstUdpData.inAddr.sin_addr.s_addr = onbuf[i].srcaddr;
           dstUdpData.inAddr.sin_port = onbuf[i].srcport;
           dstUdpData.dataLen = on.length();
           mempollPtr->memDataCopy(b, &dstUdpData, getUdpDataItermHeadLen());
           mempollPtr->memDataCopy(b, on.getDataPtr(), on.length());
           LLIST_ADD_TAIL(out, &b->list);
        }
        lastRemainTaskType = IMMESG_USER_BROAD;
    }else{
        lastRemainTaskType = IMMESG_NONE;
        lastEndPtr = 0;
    }
    return n > 0 ? 1:0;
}















