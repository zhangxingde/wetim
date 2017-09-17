#include "immessagequeue.h"
#include "immessage_def.h"

imMessageQueue_c::imMessageQueue_c(int size)
    :maxQueueSize(size),semFree(size), semUse(0)
{
    mesgeQueuePtr = new MessageCell_t[maxQueueSize];
    indexFree = 0;
    indexUse = 0;
}

imMessageQueue_c::~imMessageQueue_c()
{
    delete[] mesgeQueuePtr;
}

bool imMessageQueue_c::pushMesg(const ImmessageData &src, const QString &addr, int port, int dir, int isTcp)
{
    MessageCell_t *p = 0;

    semFree.acquire();
    mutexFree.lock();
    p = &mesgeQueuePtr[indexFree];
    p->m = src;
    p->addr = addr;
    p->port = port;
    p->dir = dir;
    p->isTcp = isTcp;
    if (++indexFree >= maxQueueSize)
        indexFree = 0;
    mutexFree.unlock();
    semUse.release();
    return 1;
}

bool imMessageQueue_c::getMesg(ImmessageData &dst, QString &addr, int &port, int &dir, int &isTcp)
{
    MessageCell_t *p = 0;

    semUse.acquire();
    mutexUse.lock();
    p = &mesgeQueuePtr[indexUse];
    dst = p->m;
    addr = p->addr;
    port = p->port;
    dir = p->dir;
    isTcp = p->isTcp;
    //mesgeQueuePtr[indexUse].deStroyMem();//由于队列中的对象不会执行析构函数，无法释放对内存的引用计数。需要手动进行释放
    if (++indexUse >= maxQueueSize)
        indexUse = 0;
    mutexUse.unlock();
    semFree.release();
    return 1;
}
