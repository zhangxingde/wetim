#ifndef IMCHATCLIENT_H
#define IMCHATCLIENT_H
#include <map>
#include <vector>
#include <list>
#include "poll/epollprocess.h"
#include "poll/sockhandle.h"
#include "include_h/singletontemp.h"
#include "sevsqldb.h"

class ImmessageData;
class ClientTcpImmesgEvent : public tcpSockHandle_c
{
public:
    ClientTcpImmesgEvent(memPool_c *mempoolptr);
    void setSqlDbPtr (SevSqlDB *db) {mdbptr =db;}
private:
	ll_list_t imMessageQueue;
    ll_list_t inDataQueue, outDataQueue;
    uint32_t  rcvMesgLen;
    uint32_t  shouldRcvLen;
    SevSqlDB *mdbptr;

    void close();

    int doSelfWorkByRecvData(ll_list_t *head);
    int doSelfWorkBySendData(ll_list_t *head);

    int doSevByMesgType (ImmessageData &imsgData, ll_list_t *out);

    int replyCmdApplyNum (ImmessageData &imsgData, ll_list_t *out);
};

class ClientTcpAccept : public acceptSockHandle_c, public SingletonTempBase<ClientTcpAccept>
{
    friend class SingletonTempBase<ClientTcpAccept>;
public:
    typedef std::list<ClientTcpImmesgEvent*> ClientPoolPtr;
    ClientTcpImmesgEvent* getOneClientTcpEventPtr ();
    void pushInOneClientTcpEventPtr (ClientTcpImmesgEvent *p);
private:
    ClientTcpAccept();
    ~ClientTcpAccept();

    const unsigned int poolSize;
    ClientPoolPtr clipoolptr;


    memPool_c mem;
    SevSqlDB sqlite3db;
    ClientTcpImmesgEvent *clientsPtr;
    void setupClientPool ();
    void destroyClientPool ();

    sockHandle_c* accept(int sock, struct sockaddr_in *addr);
};

#if 0
class imChatUsrStatus_c;
class imMessageMan_c;

class imChatTcpClient_c : public tcpSockHandle_c
{
public:
    imChatTcpClient_c(int sock, struct sockaddr_in *addr):
        tcpSockHandle_c(sock, addr),outDataQueue(&chatMessageMemPool)
    {
        //LLIST_INIT(&outDataQueueHead);
    }
    ~imChatTcpClient_c();
    static imChatUsrStatus_c* findUsrStautsByUn (int un);
    int pushData2Send (const void *src, int len);
    const struct sockaddr_in& getRemAddr () {return inaddr;}
private:
    typedef std::map<int, imChatUsrStatus_c*> imChatUsrOnlineMap_t;

    static imChatUsrOnlineMap_t usrOnlineMap;
    static memPool_c chatMessageMemPool;

    int pushData2OutQueue (const void *src, int len);
    int doSelfWorkByRecvData(const void *m, int len);
    int doSelfWorkBySendData(void *dst, int len);

    int addNewUseWentOn (imMessageMan_c &mesg);
    int progressP2Pdata (imMessageMan_c &mesg);

    int getTotalUsrOnlineStaus (imMesgOnLineList_c &mesg);
    void broadMessage (const void *src, int len);


    mesgQueue_c outDataQueue;
    int usrNu;
};

class imChatUdpClient_c : public udpSockHandle_c
{
public:
    typedef struct {
        netAddr_t dstaddr;
        int dataLen;
        char dataContent[1400];
    }chatUdpDataItrem_t;

    imChatUdpClient_c(int sock);
    ~imChatUdpClient_c();

    int pushData2OutQueue (const void *m, int len, sockaddr_in *outaddr);
private:
    int doSelfWorkByRecvData(const udpDataIterm_t *src);
    int doSelfWorkBySendData(udpDataIterm_t *dst);

    memPool_c dataMemPool;
    mesgQueue_c outDataQueue;
};

class imChatTcpListen_c : public acceptSockHandle_c
{
public:
    imChatTcpListen_c(int sock, struct sockaddr_in *addr):
        acceptSockHandle_c(sock, addr)
    {}
private:
    sockHandle_c* accept(int sock, sockaddr_in *addr);
};
#endif
#endif // IMCHATCLIENT_H

