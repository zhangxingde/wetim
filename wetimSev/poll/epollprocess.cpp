#include <unistd.h>
#include <sys/epoll.h>
#include <utility>
#include "epollprocess.h"


sockRotating_c* sockRotating_c::getSockRotaingPtr (int size)
{
#ifndef POLL_SELECT
	return new epollProcess_c(size);
#else
	return new selectProcess_c();
#endif
}

#ifndef POLL_SELECT
epollProcess_c::epollProcess_c(int pollSize)
    :maxPollSzie(pollSize)//, totalSock(maxPollSzie, 0)
{
    epollfd = epoll_create(maxPollSzie);
    //totalSock.clear();
}

epollProcess_c::~epollProcess_c()
{
    ::close(epollfd);
}

bool epollProcess_c::addSock2Rotating(sockHandle_c *p)
{
    struct epoll_event e;
    bool b = 0;

    if (!p)
        return b;
    e.data.ptr = p;
    e.events = EPOLLIN | EPOLLET ;
    if ((b = (epoll_ctl(epollfd, EPOLL_CTL_ADD, p->getSockFd(), &e) == 0))){
        b = addSock2RotatingQueue(p);
    }
    return b;
}

bool epollProcess_c::modifySock2Rotating(sockHandle_c *p, bool r, bool w)
{
    struct epoll_event e;
    bool b = 0;

    if (!p)
        return b;

    e.data.ptr = p;
    e.events  = 0;
    if (r)
        e.events |= EPOLLIN | EPOLLET;
    if (w)
        e.events |= EPOLLOUT | EPOLLET;
    return epoll_ctl(epollfd, EPOLL_CTL_MOD, p->getSockFd(), &e) == 0;
}

bool epollProcess_c::delSock2Rotating(sockHandle_c *p)
{
    struct epoll_event e;
    int sock = p->getSockFd();

    if (LLIST_EMPTY(&sockRotatingQueueHead))
        return 0;
    removeSockFromRotatinQueue(p);
    p->close();
    return epoll_ctl(epollfd, EPOLL_CTL_DEL, sock, &e) == 0;
}

bool epollProcess_c::addSock2RotatingQueue(sockHandle_c *p)
{
	ll_list_t *l = p->getQueueListPtr();

    if (!l)
        return 0;
    LLIST_INIT(l);
    LLIST_ADD_NEXT(&sockRotatingQueueHead, l);
    return 1;
}

void epollProcess_c::removeSockFromRotatinQueue(sockHandle_c *p)
{
    LLIST_DEL(p->getQueueListPtr());
}

//#include <iostream>
int epollProcess_c::startRotating()
{
    struct epoll_event *activeEventPtr;
    int nfds;
    sockHandle_c *s;

    if (epollfd == -1){
        return 1;
    }
    activeEventPtr = new struct epoll_event[maxPollSzie];
    while (1){
        nfds = epoll_wait(epollfd, activeEventPtr, maxPollSzie, -1);
        for (int x = 0, r = 0; x < nfds; ++x){
            s = (sockHandle_c*)activeEventPtr[x].data.ptr;
            if (activeEventPtr[x].events & EPOLLIN){
                //std::cout<<"in sock = "<<s->sockfd<<std::endl;
                r = s->sockrecv(*this);
                if (r < 0){
                    delSock2Rotating(s);
                    continue;
                }
            }
            if (activeEventPtr[x].events & EPOLLOUT){
                //std::cout<<"out sock = "<<s->sockfd<<std::endl;
                r = s->socksend(*this);
                if (r < 0){
                    delSock2Rotating(s);
                }
            }

        }
    }
    delete [] activeEventPtr;
}
#else
/////////////////////////////
/////////////////////////////
selectProcess_c::selectProcess_c()
{
    //LLIST_INIT(&sockRotatingQueueHead);
}

selectProcess_c::~selectProcess_c(){}

bool selectProcess_c::addSock2Rotating(sockHandle_c *p)
{
	sockHandle_c::sockLlist_t *l = MEMBER_ENTRY(p->getQueueListPtr(), sockHandle_c::sockLlist_t, list);

    LLIST_INIT(&l->list);
    l->r = 1;
    l->w = 0;
    LLIST_ADD_NEXT(&sockRotatingQueueHead, &l->list);
    return 1;
}

bool selectProcess_c::modifySock2Rotating(sockHandle_c *p, bool r, bool w)
{
	sockHandle_c::sockLlist_t *l = MEMBER_ENTRY(p->getQueueListPtr(), sockHandle_c::sockLlist_t, list);

    l->r = r;
    l->w = w;
    return 1;
}

bool selectProcess_c::delSock2Rotating(sockHandle_c *p)
{
    if (LLIST_EMPTY(&sockRotatingQueueHead))
        return 0;
	sockHandle_c::sockLlist_t *l = MEMBER_ENTRY(p->getQueueListPtr(), sockHandle_c::sockLlist_t, list);
    LLIST_DEL(&l->list);
    p->close();
    return 1;
}

bool selectProcess_c::addSock2Fdset(int &maxfd, fd_set &r, fd_set &w)
{
    ll_list_t *l = sockRotatingQueueHead.next;
	sockHandle_c::sockLlist_t *sl = 0;

    if (LLIST_EMPTY(&sockRotatingQueueHead)){
        return 0;
    }
    do {
        sl = MEMBER_ENTRY(l, sockHandle_c::sockLlist_t, list);
        if (sl->r){
            FD_SET(sl->p->getSockFd(), &r);
        }
        if (sl->w){
            FD_SET(sl->p->getSockFd(), &w);
        }
        if ((sl->r | sl->w) && maxfd < sl->p->getSockFd()){
            maxfd = sl->p->getSockFd();
        }
        l = l->next;
    }while (l != &sockRotatingQueueHead);
    return 1;
}

int selectProcess_c::startRotating()
{
    fd_set rset,wset;
    int n, maxfd;
    struct timeval rotaTime = {5,0};

    while (1){
        maxfd = 0;
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if (!addSock2Fdset(maxfd, rset, wset))
            return 0;
        n = select(maxfd + 1, &rset, &wset, NULL, &rotaTime);
        if (n <= 0){
            rotaTime.tv_sec = 30;
        }else{
            ll_list_t *l = sockRotatingQueueHead.next;
			sockHandle_c::sockLlist_t *sl;

            while (l != &sockRotatingQueueHead){
                sl = MEMBER_ENTRY(l, sockHandle_c::sockLlist_t, list);
                if (FD_ISSET(sl->p->getSockFd(), &rset)){
                    if (sl->p->sockrecv(*this) < 0){
                        l = l->next;
                        delSock2Rotating(sl->p);
                        continue;
                    }
                }
                if (FD_ISSET(sl->p->getSockFd(), &wset)){
                    if (sl->p->socksend(*this) < 0){
                        l = l->next;
                        delSock2Rotating(sl->p);
                        continue;
                    }
                }
                l = l->next;
            }
        }
    }
}
#endif
