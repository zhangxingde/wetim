#include <string.h>
#include "messagequeue.h"

mesgQueue_c::mesgQueue_c(memPool_c *queueMemPoolPtr)
{
    LLIST_INIT(&outDataQueueHead);
    this->queueMemPoolPtr = queueMemPoolPtr;
}

mesgQueue_c::~mesgQueue_c()
{
    cleanAll();
}

void mesgQueue_c::cleanAll()
{
   memBbuff_t *p;

    while (!LLIST_EMPTY(&outDataQueueHead)){
        p = MEMBER_ENTRY(outDataQueueHead.next, memBbuff_t, list);
        LLIST_DEL(&p->list);
        queueMemPoolPtr->freeMembuf(p);
    }
}

int mesgQueue_c::pushData2Queue(const void *src, int len, int istcpData)
{
    memBbuff_t *lastonememptr;
    int cpLen = 0, l;

    if (!src || len <= 0)
        return 0;
    if (istcpData && !LLIST_EMPTY(&outDataQueueHead)){
        lastonememptr = MEMBER_ENTRY(outDataQueueHead.pre, memBbuff_t, list);
        l = memPool_c::memDataCopy(lastonememptr, src, len);
        len -= l;
        src = (const char*)src + l;
        cpLen += l;
    }
    while (len > 0 &&(lastonememptr = queueMemPoolPtr->getMembuf())){
        l = memPool_c::memDataCopy(lastonememptr, src, len);
        len -= l;
        src = (const char*)src + l;
        cpLen += l;
        LLIST_ADD_TAIL(&outDataQueueHead, &lastonememptr->list);
    }
    return cpLen;
}

int mesgQueue_c::pullQueue2Data(void *dst, int len, int istcpData)
{
    int cpLen = 0, size;
    memBbuff_t *p;

   while (!LLIST_EMPTY(&outDataQueueHead) && len > 0){
        p = MEMBER_ENTRY(outDataQueueHead.next, memBbuff_t, list);
        size = p->dataEndPtr - p->dataBeginPtr;
		size = size > len ? len : size;
        memcpy((char*)dst + cpLen, p->dataBeginPtr, size);
        cpLen += size;
        len -= size;
		p->dataBeginPtr += size;
		if (!istcpData || ( p->dataBeginPtr >= p->dataEndPtr)){//no data or udp
			LLIST_DEL(&p->list);
			queueMemPoolPtr->freeMembuf(p);
		}
    }
    return cpLen;
}
