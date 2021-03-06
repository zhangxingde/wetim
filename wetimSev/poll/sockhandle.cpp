﻿#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <iostream>
#include "sockhandle.h"
#include "epollprocess.h"

sockHandle_c::sockHandle_c()
{
    memset(&inaddr, 0, sizeof(inaddr));
    sockfd = -1;
    LLIST_INIT(&list.list);
	list.w = list.r = 0;
	list.p = this;
}

sockHandle_c::~sockHandle_c()
{
    ::close(sockfd);
    //std::cout<<"close sock = "<<sockfd<<std::endl;
}

void sockHandle_c::setSockInetAddr(int sock, const sockaddr_in *addr)
{
    sockfd = sock;
    if (addr)
        inaddr = *addr;
    setNonBlockSock(sockfd);
}

int sockHandle_c::getTcpListenSock(const char *listenAddrPtr, unsigned int listenPort, int listenSize)
{
    int f, on = 1;
    struct sockaddr_in inaddr;

    if ((f = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
        return f;
    }
    memset(&inaddr, 0, sizeof (inaddr));
    inaddr.sin_family = AF_INET;
    inaddr.sin_port = htons(listenPort);
    if (!listenAddrPtr)
        inaddr.sin_addr.s_addr = INADDR_ANY;
    else
        inaddr.sin_addr.s_addr = inet_addr(listenAddrPtr);
    setsockopt(f, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
    if (bind (f, (struct sockaddr*)&inaddr, sizeof (inaddr)) != 0){
       ::close (f);
       return -1;
    }
    if (listen(f, listenSize) != 0){
        ::close(f);
        return -1;
    }
    signal(SIGPIPE, SIG_IGN);
    return f;
}

int sockHandle_c::getUdpSockOnPort(const char *addr, unsigned short port)
{
    int f, on = 1;
    struct sockaddr_in inaddr;

    if ((f = socket(AF_INET, SOCK_DGRAM, 0)) <= 0){
        return f;
    }
    memset(&inaddr, 0, sizeof (inaddr));
    inaddr.sin_family = AF_INET;
    inaddr.sin_port = htons(port);
    if (!addr)
        inaddr.sin_addr.s_addr = INADDR_ANY;
    else
        inaddr.sin_addr.s_addr = inet_addr(addr);
    setsockopt(f, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));
    if (bind (f, (struct sockaddr*)&inaddr, sizeof (inaddr)) != 0){
       ::close (f);
       return -1;
    }
    signal(SIGPIPE, SIG_IGN);
    return f;
}

void sockHandle_c::close()
{
    ::close(sockfd);
}

void sockHandle_c::setNonBlockSock(int sock)
{
    int f = fcntl(sock, F_GETFL);

    fcntl(sock, F_SETFL, f | O_NONBLOCK);
}

//////////////////////////////////////////////////////

int acceptSockHandle_c::sockrecv(sockRotating_c &poll)
{
    int s;
    struct sockaddr_in inaddr;
    socklen_t len = sizeof (inaddr);
    sockHandle_c *t;

    while ((s = ::accept(sockfd, (struct sockaddr*)&inaddr, &len)) > 0){
        t = this->accept(s, &inaddr);
		if (!poll.addSock2Rotating(t)){
            ::close(s);
			delete t;
		}
    }
    return 0;
}

int acceptSockHandle_c::socksend(sockRotating_c &poll) {return 0;}

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////

tcpSockHandle_c::tcpSockHandle_c()
    :sockHandle_c()
{
	LLIST_INIT(&sendQueueListHead);
    mempollPtr = 0;
}


int tcpSockHandle_c:: sockrecv(sockRotating_c &poll)
{
	memBbuff_t *b = 0;
	ll_list_t *head = 0;
    int len = 0, r;

	do {
		if (len == 0){
			b = mempollPtr->getMembuf();
			len = b->memTailPtr - b->dataEndPtr;
			if (head)
				LLIST_ADD_TAIL(head, &b->list);
			else
				head = &b->list;
		}
		if ((r = recv(sockfd, b->dataEndPtr, len, 0)) > 0){
			b->dataEndPtr += r;
		}
		len = b->memTailPtr - b->dataEndPtr;
	}while (r > 0);
	if ((r < 0 && (errno != EAGAIN)) || (r == 0)){
		mempollPtr->freeMemList(head);
		return -1;
	}
    if ((r = doSelfWorkByRecvData(head)) > 0){
		poll.modifySock2Rotating(this, 1,1);
    }else if (r == 0){
        poll.modifySock2Rotating(this, 1, 0);
    }
    return r;
}

int tcpSockHandle_c:: socksend(sockRotating_c &poll)
{
    int r, f, len = 0;
	memBbuff_t *p = 0;
	ll_list_t *head = 0;

	if (LLIST_EMPTY(&sendQueueListHead)){//等到把上次的历史数据发送完毕后，再从新取数据
		f = doSelfWorkBySendData(&sendQueueListHead);
		if (LLIST_EMPTY(&sendQueueListHead)){
			poll.modifySock2Rotating(this, 1, 0);
			return f;
		}
	}
	do {
		if (len == 0 && !LLIST_EMPTY(&sendQueueListHead)){
			p = MEMBER_ENTRY(sendQueueListHead.next, memBbuff_t, list);
			len = p->dataEndPtr - p->dataBeginPtr;
			LLIST_DEL(&p->list);
			if (head)
				LLIST_ADD_TAIL(head, &p->list);
			else
				 head = &p->list;
		}
		if ((r = send(sockfd, p->dataBeginPtr, len, 0)) > 0){
			p->dataBeginPtr += r;
		}
		len = p->dataEndPtr - p->dataBeginPtr;
	}while (r > 0);
	mempollPtr->freeMemList(head);
    if (r < 0 && (errno != EAGAIN)){
        return -1;
    }
    poll.modifySock2Rotating(this,1, 1);
    return f;
}
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////

udpSockHandle_c::udpSockHandle_c()
    :sockHandle_c()
{
    mempollPtr = 0;
    LLIST_INIT(&sendQueueListHead);
}


int udpSockHandle_c:: sockrecv(sockRotating_c &poll)
{
    memBbuff_t *b = 0;
    ll_list_t *head = 0;
    int len = 0, r;
    udpDataIterm_t *p = 0;

    do {
        if (len == 0){
            b = mempollPtr->getMembuf();
            p = (udpDataIterm_t*)b->s;
            memset(p, 0, sizeof (*p));
            p->addrLen = sizeof(p->inAddr);
            b->dataEndPtr = p->dataChar;
            len = b->memTailPtr - b->dataEndPtr;
            if (head)
                LLIST_ADD_TAIL(head, &b->list);
            else
                head = &b->list;
        }
        if ((r = recvfrom(sockfd, b->dataEndPtr, len, 0, (struct sockaddr*)&p->inAddr, &p->addrLen)) > 0){
            b->dataEndPtr += r;
            p->dataLen += r;
        }
        len = b->memTailPtr - b->dataEndPtr;
    }while (r > 0);
    if ((r < 0 && (errno != EAGAIN)) || (r == 0)){
        mempollPtr->freeMemList(head);
        return -1;
    }
    if ((r = doSelfWorkByRecvData(head)) > 0){
        poll.modifySock2Rotating(this, 1,1);
    }else if (r == 0){
        poll.modifySock2Rotating(this, 1, 0);
    }
    return r;
}
#include "stdebug.h"
int udpSockHandle_c:: socksend(sockRotating_c &poll)
{
    int r, f, len = 0;
    memBbuff_t *p = 0;
    ll_list_t *head = 0;
    udpDataIterm_t *u = 0;

    if (LLIST_EMPTY(&sendQueueListHead)){//等到把上次的历史数据发送完毕后，再从新取数据
        f = doSelfWorkBySendData(&sendQueueListHead);
        if (LLIST_EMPTY(&sendQueueListHead)){
            poll.modifySock2Rotating(this, 1, 0);
            return f;
        }
    }
    do {
        if (len == 0 && !LLIST_EMPTY(&sendQueueListHead)){
            p = MEMBER_ENTRY(sendQueueListHead.next, memBbuff_t, list);
            u = (udpDataIterm_t*)p->s;
            p->dataBeginPtr = u->dataChar;
            len = p->dataEndPtr - p->dataBeginPtr;
            LLIST_DEL(&p->list);
            if (head)
                LLIST_ADD_TAIL(head, &p->list);
            else
                 head = &p->list;
        }
        r = 0;
        if (len > 0 && (r = sendto(sockfd, p->dataBeginPtr, len, 0, (struct sockaddr*)&u->inAddr, u->addrLen)) > 0){
            p->dataBeginPtr += r;
        }
        len = p->dataEndPtr - p->dataBeginPtr;
    }while (r > 0);
    mempollPtr->freeMemList(head);
    if (r < 0 && (errno != EAGAIN)){
        return -1;
    }
    poll.modifySock2Rotating(this,1, 1);
    return f;
}

