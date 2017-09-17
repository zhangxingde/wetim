#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include "llist.h"
#include "mempool.h"

class mesgQueue_c {
public:
    mesgQueue_c(memPool_c *queueMemPoolPtr);
    ~mesgQueue_c();

    int pushData2Queue(const void *src, int len, int istcpData = 1);
    int pullQueue2Data(void *dst, int len, int istcpData = 1);
private:
    memPool_c *queueMemPoolPtr;
    ll_list_t outDataQueueHead;

    void cleanAll ();
};
//
#endif // MESSAGEQUEUE_H
