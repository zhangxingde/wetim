#ifndef CLIENUDPEVENT_H
#define CLIENUDPEVENT_H
#include "poll/epollprocess.h"
#include "poll/sockhandle.h"
#include "sevsqldb.h"

class ImmessageData;
class ClientUdpImmesgEvent : public udpSockHandle_c
{
public:
    ClientUdpImmesgEvent();
    ~ClientUdpImmesgEvent();
    void close();
private:
    ll_list_t inDataQueue, outDataQueue;
    SevSqlDB *mdbptr;
    int lastRemainTaskType;

    int doSelfWorkByRecvData(ll_list_t *head);
    int doSelfWorkBySendData(ll_list_t *head);

    int doSevByMesgType (struct sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p);

    int replyCmdKeepAlive (struct sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p);
    int replyCmdBroadWentOn (struct sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p);
    int replyCmdGetFrdUdpAddr (struct sockaddr_in &srcaddr, const ImmessageData &imsgData, ll_list_t *out, void **p);

    int doTheLastRemainTask (int cmdtpe, void *p, ll_list_t *out);//有任务返回1，否则返回0
    int loopBroadUsrWenton (void *p, ll_list_t *out);


};
#endif // CLIENUDPEVENT_H
