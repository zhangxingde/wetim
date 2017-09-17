#ifndef EPOLLPROCESS_H
#define EPOLLPROCESS_H
#include "llist.h"
#include "sockhandle.h"

class sockRotating_c {
public:
    sockRotating_c()
    {
        LLIST_INIT(&sockRotatingQueueHead);
    }
    virtual ~sockRotating_c(){}
    virtual bool addSock2Rotating (sockHandle_c *p) = 0;
    virtual bool modifySock2Rotating (sockHandle_c *p, bool r, bool w) = 0;
    virtual bool delSock2Rotating (sockHandle_c *p) = 0;
    virtual int startRotating () = 0;
	static sockRotating_c* getSockRotaingPtr (int size = 0);
protected:
    ll_list_t sockRotatingQueueHead;
};

#ifndef POLL_SELECT
class epollProcess_c : public sockRotating_c
{
public:
    epollProcess_c(int pollSize);
    ~epollProcess_c();

    bool addSock2Rotating (sockHandle_c *p);
    bool modifySock2Rotating (sockHandle_c *p, bool r, bool w);
    bool delSock2Rotating (sockHandle_c *p);
    int startRotating();
private:
    const int maxPollSzie;
    int epollfd;

    bool addSock2RotatingQueue (sockHandle_c *p);
    void removeSockFromRotatinQueue (sockHandle_c *p);
};

/////////////////////////////
/////////////////////////////
#else
#include <sys/select.h>


class selectProcess_c : public sockRotating_c
{
public:
    selectProcess_c ();
    ~selectProcess_c();
    bool addSock2Rotating (sockHandle_c *p);
    bool modifySock2Rotating (sockHandle_c *p, bool r, bool w);
    bool delSock2Rotating (sockHandle_c *p);
    int startRotating();
private:

    bool addSock2Fdset (int &maxfd, fd_set &r, fd_set &w);
};
#endif
#endif // EPOLLPROCESS_H

