/*
 * sockHandle_c作用：
 * 1.自己收发数据
 * 2.由具体的第三方业：务处理数据
*/
#ifndef SOCKHANDLE_H
#define SOCKHANDLE_H
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "llist.h"
#include "mempool.h"

class sockRotating_c;

class sockHandle_c {
public:
    typedef struct {
        ll_list_t list;
        int r;
        int w;
        sockHandle_c *p;
    }sockLlist_t;

    sockHandle_c();
    virtual ~sockHandle_c ();
    static int getTcpListenSock (const char *listenAddrPtr, unsigned int listenPort, int listenSize);
    static int getUdpSockOnPort (const char *addr ,unsigned short port);

    void setSockInetAddr (int sock, const sockaddr_in *addr);
    int getSockFd () const {return sockfd;}
	ll_list_t* getQueueListPtr () {return &list.list;}
    virtual void close ();

    virtual int sockrecv (sockRotating_c &poll) = 0;//成功 返回 0,错误返回 -1,将关闭连接
    virtual int socksend (sockRotating_c &poll) = 0;
protected:
    int sockfd;
    //sockRotating_c *sockRotatingObjPtr;
    struct sockaddr_in inaddr;

    //bool addSock2Rotating (sockHandle_c *p);
    //bool modifySock2Rotating(bool r, bool w);
private:
    void setNonBlockSock (int sock);
    sockLlist_t list;
};
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////
class acceptSockHandle_c : public sockHandle_c
{
public:

protected:
    int sockrecv(sockRotating_c &poll);
    int socksend(sockRotating_c &poll);
    virtual sockHandle_c* accept(int sock, struct sockaddr_in *addr) = 0;

};
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

class tcpSockHandle_c : public sockHandle_c
{
public:
    tcpSockHandle_c();
	void setMemPoolPtr (memPool_c *p) {mempollPtr = p;}

protected:
	memPool_c *mempollPtr;

    virtual int doSelfWorkByRecvData (ll_list_t *head) = 0;
    /*int r = doSelfWorkByRecvData()
     *r > 0 , some data will be send, doSelfWorkBySendData() will be called by epoll;
     *r == 0 , no data to send, and will to linsten data in;
     *r < 0 , do something error, will to close this connect;
    */
    virtual int doSelfWorkBySendData (ll_list_t *head) = 0;
    /*int len = doSelfWorkBySendData()
     *len > 0 , continue to send the data in len length, and doSelfWorkBySendData() will be called by epoll one more;
     *len == 0 , no more data to send, and will to linsten data in;
     *len < 0 , do something error, will to close this connect;
    */

private:
    int sockrecv(sockRotating_c &poll);
    int socksend(sockRotating_c &poll);

	ll_list_t sendQueueListHead;
};
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////

class udpSockHandle_c : public sockHandle_c
{
public:
    #pragma pack(1)
    typedef struct {
        struct sockaddr_in inAddr;
        socklen_t addrLen;
        unsigned int dataLen;
        char dataChar[1];
    }udpDataIterm_t;
    #pragma pack()

    udpSockHandle_c();
    void setMemPoolPtr (memPool_c *p) {mempollPtr = p;}

protected:
    memPool_c *mempollPtr;
    int getUdpDataItermHeadLen () {return sizeof(udpDataIterm_t) - 1;}

    virtual int doSelfWorkByRecvData (ll_list_t *head) = 0;
    /*int r = doSelfWorkByRecvData()
     *r > 0 , some data will be send, doSelfWorkBySendData() will be called by epoll;
     *r == 0 , no data to send, and will to linsten data in;
     *r < 0 , do something error, will to close this connect;
    */
    virtual int doSelfWorkBySendData (ll_list_t *head) = 0;
    /*int len = doSelfWorkBySendData()
     *len > 0 , continue to send the data in len length, and doSelfWorkBySendData() will be called by epoll one more;
     *len == 0 , no more data to send, and will to linsten data in;
     *len < 0 , do something error, will to close this connect;
    */

private:
    int sockrecv(sockRotating_c &poll);
    int socksend(sockRotating_c &poll);

    ll_list_t sendQueueListHead;


};

#endif // SOCKHANDLE_H

