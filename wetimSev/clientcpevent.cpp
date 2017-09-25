#include <string.h>
#include <iostream>
#include "clientcpevent.h"
#include "immesgdecor.h"
#include "stdebug.h"
#include "usronlinestate.h"

ClientTcpAccept::ClientTcpAccept():
    poolSize(1024), mem(2048,1024)
{
    setSockInetAddr(sockHandle_c::getTcpListenSock(NULL, 12315, 1024), 0);
    sqlite3dbPtr = SevSqlDB::getInstance();
    clipoolptr.clear();
    setupClientPool();
}

ClientTcpAccept::~ClientTcpAccept()
{
    destroyClientPool();
    sqlite3dbPtr->close();
}

void ClientTcpAccept::setupClientPool()
{
    for (unsigned i = 0; i < poolSize;++i){
        clipoolptr.push_back(new ClientTcpImmesgEvent(&mem));
    }
}

void ClientTcpAccept::destroyClientPool()
{
    while (!clipoolptr.empty()){
        delete clipoolptr.front();
        clipoolptr.pop_front();
    }
}

ClientTcpImmesgEvent* ClientTcpAccept::getOneClientTcpEventPtr()
{
    ClientTcpImmesgEvent *p = 0;

    if (!clipoolptr.empty()){
        p = clipoolptr.front();
        clipoolptr.pop_front();
    }
    return p;
}

void ClientTcpAccept::pushInOneClientTcpEventPtr(ClientTcpImmesgEvent *p)
{
    clipoolptr.push_back(p);
}

sockHandle_c* ClientTcpAccept::accept(int sock, sockaddr_in *addr)
{
    ClientTcpImmesgEvent *p = getOneClientTcpEventPtr();

    if (p){
        p->setSockInetAddr(sock, addr);
        p->setMemPoolPtr(&mem);
        p->setSqlDbPtr(sqlite3dbPtr);
    }
    return p;
}



ClientTcpImmesgEvent::ClientTcpImmesgEvent(memPool_c *mempoolptr):
        tcpSockHandle_c()
{
    setMemPoolPtr(mempoolptr);
	LLIST_INIT(&imMessageQueue);
    LLIST_INIT(&inDataQueue);
    LLIST_INIT(&outDataQueue);
    shouldRcvLen = rcvMesgLen = 0;
    mdbptr = 0;
    lastRemainTaskType = -1;
}

void ClientTcpImmesgEvent::close()
{
    ClientTcpAccept *p = ClientTcpAccept::getInstance();

    p->pushInOneClientTcpEventPtr(this);
    SingletonTempBase<ClientTcpAccept>::close();
    sockHandle_c::close();
    if (!LLIST_EMPTY(&inDataQueue)){
        ll_list_t * l= inDataQueue.next;
        LLIST_DEL(&inDataQueue);
        mempollPtr->freeMemList(l);
    }
    if (!LLIST_EMPTY(&outDataQueue)){
        ll_list_t * l= outDataQueue.next;
        LLIST_DEL(&outDataQueue);
        mempollPtr->freeMemList(l);
    }
    lastRemainTaskType = -1;
}

int ClientTcpImmesgEvent::doSelfWorkByRecvData(ll_list_t *head)
{
    memBbuff_t *b;
    Immessage_t *m;
    int retnum = 0, isDel = 0;

    LLIST_ADDLINK(&inDataQueue, head);
    while (!LLIST_EMPTY(&inDataQueue)) {
        b = MEMBER_ENTRY(inDataQueue.next, memBbuff_t, list);
        m = (Immessage_t*)b->s;
        if (shouldRcvLen == 0){//begin
            rcvMesgLen = 0;
            if (ntohl(m->checkNum1) == IMMESG_CHCK1){
                if (ntohl(m->mesgLen) > IMMESG_MESGDATA_MAXLEN){
                    isDel = 1;
                }else{
                    shouldRcvLen = ntohl(m->mesgLen) + sizeof(Immessage_t);
                }
            }else{//error
                isDel = 1;
            }
        }
        if (!isDel){
            LLIST_DEL(&b->list);
            LLIST_ADD_TAIL(&outDataQueue, &b->list);
            rcvMesgLen += b->dataEndPtr - b->dataBeginPtr;
            if (rcvMesgLen >= shouldRcvLen || rcvMesgLen > IMMESG_MESGDATA_MAXLEN){//recv one all data
                retnum = 1;
                shouldRcvLen = rcvMesgLen = 0;//忽略粘包的情况。
                break;
            }
        }else{
            LLIST_DEL(&b->list);
            mempollPtr->freeMembuf(b);
            retnum = -1;
        }
    }
    STD_DEBUG("recv ret = %d", retnum);
    return retnum;
}

int ClientTcpImmesgEvent::doSelfWorkBySendData(ll_list_t *head)
{
    uint32_t mesgDataLen = 0, shouldDataLen = 0, shouldMoreLen = 0, moreLen = 0;
    memBbuff_t *b;
    Immessage_t *m;
    ImmessageData messageData;
    int retnum = 0;

    if (doTheLastRemainTask(lastRemainTaskType, NULL,head)){
        return 1;
    }

    while (!LLIST_EMPTY(&outDataQueue)){
        b = MEMBER_ENTRY(outDataQueue.next, memBbuff_t, list);
        m = (Immessage_t*)b->s;
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
        return -1;
    }
    retnum = doSevByMesgType(messageData, head);
    STD_DEBUG("out ret = %d",retnum);
    return retnum;
}

int ClientTcpImmesgEvent::doSevByMesgType(const ImmessageData &imsgData, ll_list_t *out)
{
    switch (imsgData.getMesgType()){
        case IMMESG_APPLYNUM:
            return replyCmdApplyNum(imsgData, out);
        case IMMESG_USER_LOGON:
            return replyCmdLogonAuth(imsgData, out);
        case IMMESG_USER_ONLIST:
            return replyCmdUsrOnlist(imsgData, out);
    }
    return -1;
}

int ClientTcpImmesgEvent::replyCmdApplyNum(const ImmessageData &imsgData, ll_list_t *out)
{
    ImmessageData m(IMMESG_APPLYNUM);
    ImmesgDecorApplyNum apply(&m);
    ImmesgDecorApplyNum::ApplyUsrInfos_t userinfo;

    userinfo.uid = mdbptr->getNewUsrID();
    if (userinfo.uid <= 0)
        return 0;
    if (!mdbptr->getUsrNameByUsrID(userinfo.uid, userinfo.name, sizeof(userinfo.name)) ||
            !mdbptr->getUsrPasswdByUsrID(userinfo.uid, userinfo.passwd, sizeof(userinfo.passwd)) ||
            (userinfo.avicon = mdbptr->getAvaiconIdByUsrID(userinfo.uid)) < 0){
        return -1;
    }
    userinfo.uid = htonl(userinfo.uid);
    userinfo.avicon = htonl(userinfo.avicon);
    apply.setDstSrcUsr(userinfo.uid, 0);
    apply.addMesgData(sizeof(userinfo),&userinfo);
    ll_list_t *l = mempollPtr->fflushData2QueueLList(apply.getDataPtr(), apply.length());
    if (l){
        LLIST_ADD_TAIL(out, l);
        return 1;
    }
    return 0;
}

int ClientTcpImmesgEvent::replyCmdLogonAuth(const ImmessageData &imsgData, ll_list_t *out)
{
    ImmesgDecorLogon logon(&imsgData);
    ImmessageData m(IMMESG_USER_LOGON);
    ImmesgDecorLogon re(&m);
    char passwd[32];

    if (mdbptr->getUsrPasswdByUsrID(logon.getSrcUsrId(), passwd, sizeof(passwd)) &&
            !strcmp(passwd, logon.getPassword())){
        re.setAuthSucc(1);
    }else{
        re.setAuthSucc(0);
    }
    re.setDstSrcUsr(logon.getSrcUsrId(), 0);
    ll_list_t *l = mempollPtr->fflushData2QueueLList(re.getDataPtr(), re.length());
    if (l){
        LLIST_ADD_TAIL(out, l);
        return 1;
    }
    return 0;
}

int ClientTcpImmesgEvent::replyCmdUsrOnlist(const ImmessageData &imsgData, ll_list_t *out)
{
   return loopGetAllUsrList(0,out);
}

int ClientTcpImmesgEvent::doTheLastRemainTask(int cmdtpe, void *p, ll_list_t *out)
{
    if (cmdtpe == IMMESG_NONE)
        return 0;
    else if (cmdtpe == IMMESG_USER_ONLIST){
        return loopGetAllUsrList(p,out);
    }
    return 0;
}

int ClientTcpImmesgEvent::loopGetAllUsrList(void *p, ll_list_t *out)
{
    static ll_list_t *lasEndPtr = 0;
    ll_list_t *l = 0;

    UsrOnlineState onState;
    const int maxn = 100;
    UsrOnlineState::UsrOnState_t onbuf[maxn];
    int n;

    ImmessageData m(IMMESG_USER_ONLIST);
    ImmesgDecorOnlist to(&m);

    n = onState.getOnlineUsrInfo(onbuf, maxn, &lasEndPtr);
    if (n > 0){
        for (int i = 0; i < n; ++i){
            char name[32] = {0};
            mdbptr->getUsrNameByUsrID(onbuf[i].uid, name, sizeof(name));
            to.addOneUsr(onbuf[i].uid, name, mdbptr->getAvaiconIdByUsrID(onbuf[i].uid));
        }
        l = mempollPtr->fflushData2QueueLList(to.getDataPtr(),to.length());
        LLIST_ADD_TAIL(out, l);
        lastRemainTaskType = IMMESG_USER_ONLIST;
    }else{
        lasEndPtr = 0;
        lastRemainTaskType = IMMESG_NONE;
    }
    return l ? 1:0;
}

//////////////////////////////////////////
//////////////////////////////////////////



#if 0
#include "immessage_def.h"
#include "imchatuseronline.h"
#include "immesgparse.h"

sockHandle_c* imChatTcpListen_c::accept(int sock, sockaddr_in *addr)
{
    std::cout<<"new sock "<<sock<<std::endl;
    return new imChatTcpClient_c(sock, addr);
}
/////////////////////////////////////////////
/// \brief imChatTcpClient_c::usrOnlineMap
///

memPool_c imChatTcpClient_c::chatMessageMemPool(2048, 1024);

imChatTcpClient_c::imChatUsrOnlineMap_t imChatTcpClient_c::usrOnlineMap;

imChatTcpClient_c::~imChatTcpClient_c()
{
    imChatUsrOnlineMap_t::iterator it = usrOnlineMap.find(usrNu);

    std::cout<<"client close "<<sockfd<<"\n";
    if (it != usrOnlineMap.end()){
        imChatUsrStatus_c *p = it->second;
        imMessageMan_c exitmesg(p->getUsrNu(), IMMESG_USER_LOGOUT);

        broadMessage(exitmesg.getMemBuf(), exitmesg.length());
        delete it->second;
        usrOnlineMap.erase(it);
    }
}

int imChatTcpClient_c::pushData2Send(const void *src, int len)
{
    int cplen = pushData2OutQueue(src, len);

    if (cplen > 0 && modifySock2Rotating(1,1)){
        return cplen;
    }
    return 0;
}

static void setFromUsrStatus (imMesgOnline_t *statusPtr, imChatUsrStatus_c *u)
{
    statusPtr->remAddr.ipv4 = u->getRemAddr().sin_addr.s_addr;
    statusPtr->remAddr.port = u->getRemAddr().sin_port;
    strncpy(statusPtr->name, u->getUsrName(), sizeof (statusPtr->name));
    statusPtr->usrnu = u->getUsrNu();
}

#include <stdio.h>
int imChatTcpClient_c::pushData2OutQueue(const void *src, int len)
{
    return outDataQueue.pushData2Queue(src, len);
}

#include <stdlib.h>
int imChatTcpClient_c::doSelfWorkByRecvData(const void *m, int len)
{
    if (len <= 0)
        return 0;
    imMessageMan_c mesg(m, len);
    if (!mesg.isValid()){
        return 0;
    }
    //std::cout<<"recv  message len = "<<len<<std::endl;
    if (mesg.getMesgType() == IMMESG_USER_LOGON){
        return addNewUseWentOn(mesg);
    }else if (mesg.getMesgType() == IMMESG_DATA_P2PUDP){
        return progressP2Pdata(mesg);
    }
    return 0;
}

#include <stdio.h>
int imChatTcpClient_c::addNewUseWentOn(imMessageMan_c &mesg)
{
    imMesgOnLineList_c onlineList;

    std::cout<<"usr got on line"<<std::endl;
    if (usrOnlineMap.size() > 0 && !getTotalUsrOnlineStaus(onlineList)){
        return 0;
    }/*获取当前用户在线情况*/
    pushData2OutQueue(onlineList.getMemBuf(), onlineList.length());

    //广播新用户上线消息
    imMesgLogon_c logon(mesg);
    usrNu = logon.getUsrNu();
    imChatUsrStatus_c *p = new imChatUsrStatus_c(usrNu,this);
    p->setUsrOnlineBaseInfo(logon);
	std::cout<<"usrname="<<logon.getName()<<std::endl;

    imMesgOnline_t on;
    setFromUsrStatus(&on, p);
    imMesgOnLineList_c curUsrNew;
    curUsrNew.addOneUsrList(on);
    broadMessage(curUsrNew.getMemBuf(), curUsrNew.length());
    usrOnlineMap.insert(std::pair<int, imChatUsrStatus_c*>(usrNu, p));
    return 1;
}

#include <netinet/in.h>
#include <arpa/inet.h>
int imChatTcpClient_c::progressP2Pdata(imMessageMan_c &mesg)
{
     imMesgP2Pdata_c p(mesg);

     if (p.getp2pContent().dataType == IMMESG_P2PUDP_ASK){
         imChatUsrStatus_c *onlinusrPtr = findUsrStautsByUn(p.getp2pContent().dstUsrNu);
         std::cout<<sockfd<<" recv p2p ask message, to find "<<p.getp2pContent().dstUsrNu<<std::endl;
         if (onlinusrPtr){
             imMesgP2Pdata_c p2p_dst(p.getp2pContent().dstUsrNu, usrNu,IMMESG_P2PUDP_ASK);
             p2p_dst.pushChatData(p.getp2pContent().dataText, p.getp2pContent().dataLen);
             onlinusrPtr->pushData2Send(p2p_dst.getMemBuf(), p2p_dst.length());
         }
         return 0;
     }
     return 0;
}

int imChatTcpClient_c::doSelfWorkBySendData(void *dst, int len)
{
    return outDataQueue.pullQueue2Data(dst, len);
}

int imChatTcpClient_c::getTotalUsrOnlineStaus(imMesgOnLineList_c &mesg)
{
    imChatUsrOnlineMap_t::iterator it;
    imMesgOnline_t on;
    imChatUsrStatus_c *u;

    for (it = usrOnlineMap.begin(); (it != usrOnlineMap.end()); ++it){
        u = it->second;
        setFromUsrStatus(&on, u);
        if (!mesg.addOneUsrList(on)){
            return 0;
        }
    }
    return 1;
}

void imChatTcpClient_c::broadMessage(const void *src, int len)
{
    imChatUsrOnlineMap_t::iterator it;
    imChatUsrStatus_c *u;

    for (it = usrOnlineMap.begin(); it != usrOnlineMap.end(); ++it){
        u = it->second;
        u->pushData2Send(src, len);
        std::cout<<"bro len"<<len<<std::endl;
    }
}

imChatUsrStatus_c* imChatTcpClient_c::findUsrStautsByUn(int un)
{
    imChatUsrOnlineMap_t::iterator it = usrOnlineMap.find(un);

    if (it != usrOnlineMap.end()){
        return it->second;
    }
    return 0;
}


//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////

imChatUdpClient_c::imChatUdpClient_c(int sock):
    udpSockHandle_c(sock, 0), dataMemPool(sizeof (udpDataIterm_t)+1024, 100), outDataQueue(&dataMemPool)
{

}

imChatUdpClient_c::~imChatUdpClient_c()
{}

#include <iostream>
#include <arpa/inet.h>
int imChatUdpClient_c::doSelfWorkByRecvData(const udpDataIterm_t *src)
{
    imMessageMan_c mesg(src->dataChar, src->dataLen);

    std::cout<<"udp recv len = "<<src->dataLen<<std::endl;

    if (!mesg.isValid())
        return 0;
    if (mesg.getMesgType() == IMMESG_DATA_P2PUDP){
        imMesgP2Pdata_c p(mesg);

        if (p.getp2pContent().dataType == IMMESG_P2PUDP_ASK){
            std::cout<<p.getp2pContent().srcUsrNu<<" do IMMESG_P2PUDP_ASK = "<<std::endl;
            imMesgP2Pdata_c out(p.getp2pContent().dstUsrNu, p.getp2pContent().srcUsrNu,IMMESG_P2PUDP_ASK);
            netAddr_t udprem;

            udprem.ipv4 = src->inAddr.sin_addr.s_addr;
            udprem.port = src->inAddr.sin_port;
            out.pushChatData(p.getp2pContent().dataText, p.getp2pContent().dataLen);
            out.pushChatData(&udprem, sizeof (udprem));

            imChatUsrStatus_c *dstuser = imChatTcpClient_c::findUsrStautsByUn(p.getp2pContent().dstUsrNu);
            if (dstuser){
                dstuser->pushData2Send(out.getMemBuf(), out.length());
            }
            return 0;
        }
    }

    return 0;
}

int imChatUdpClient_c::pushData2OutQueue(const void *m, int len, struct sockaddr_in *outaddr)
{
    udpDataIterm_t dataHead;

    dataHead.addrLen = sizeof(*outaddr);
    dataHead.dataLen = len;
    dataHead.inAddr = *outaddr;
    outDataQueue.pushData2Queue(&dataHead, sizeof (dataHead), 0);
    return outDataQueue.pushData2Queue(m, len);
}

int imChatUdpClient_c::doSelfWorkBySendData(udpDataIterm_t *dst)
{
    return outDataQueue.pullQueue2Data(dst, dst->dataLen, 0);
}
#endif

















