#include <string.h>
#include "mempool.h"
#include <stdio.h>
memPool_c::memPool_c(int eachSize, int totalNum)
{
    unsigned long long int totalBufSize = (eachSize + sizeof(memBbuff_t)) * totalNum;
    pbuf = new char [totalBufSize];
    ppbuf = new char* [totalNum];

    eachBufSize = eachSize;

    char *p = pbuf;
    memBbuff_t *memHeadPtr;
    LLIST_INIT(&memBufList);
    for (int i = 0; i < totalNum; ++i){
        memHeadPtr = (memBbuff_t*)p;
        ppbuf[i] = p;
        p += eachSize + sizeof (memBbuff_t);
        memHeadPtr->dataEndPtr = memHeadPtr->dataBeginPtr;
        memHeadPtr->memTailPtr = p;
        memHeadPtr->id = i;
        LLIST_INIT(&memHeadPtr->list);
        LLIST_ADD_NEXT(&memBufList, &memHeadPtr->list);
    }
}

memPool_c::~memPool_c()
{
    if (pbuf)
        delete[] pbuf;
    if (ppbuf)
        delete[] ppbuf;
}

memBbuff_t* memPool_c::getMembuf()
{
    memBbuff_t *t = NULL;

    if (!LLIST_EMPTY(&memBufList)){
        t = MEMBER_ENTRY(memBufList.next, memBbuff_t, list);
        t->dataEndPtr = t->dataBeginPtr = t->s;
        LLIST_DEL(&t->list);
    }
    return t;
}

void memPool_c::freeMembuf(memBbuff_t *p)
{
    if (p){
		//printf ("%p %p, %p\n", p->dataBeginPtr, p->dataEndPtr, p->memTailPtr);
        LLIST_ADD_NEXT(&memBufList, &p->list);
	}
}
void memPool_c::freeMemList (ll_list_t *list)
{	
	ll_list_t * p = 0, *p2;
	memBbuff_t *t;

	if (!list){
		return;
	}
	p = list;
	do {
		p2 = p;
		p = p->next;
		t = MEMBER_ENTRY(p2, memBbuff_t, list);
		freeMembuf(t);
	}while(p != list);
}
void memPool_c::freeAnyBuf (void *p)
{
	freeMembuf((memBbuff_t*)((char*)p - sizeof(memBbuff_t) + 1));
}

int memPool_c::getTotalMemBuffNum ()
{
	int num = 0;
	ll_list_t *p = memBufList.next;

	while (p != &memBufList){
		++num;
		p = p->next;
	}
	return num;
}

int memPool_c::memDataCopy(memBbuff_t *dst, const void *src, int len)
{
    int freeLen = dst->memTailPtr - dst->dataEndPtr;
    int cpLen = freeLen > len ? len : freeLen;

    if (cpLen > 0){
        memcpy(dst->dataEndPtr, src, cpLen);
        dst->dataEndPtr += cpLen;
    }
    return cpLen;
}
int memPool_c::getDataBlockFromMemBuffList (const ll_list_t *head, const void **data)//循环调用，每次返回新的数据地址*data和其指向的长度，当返回长度为0时，
{
	return 0;
}
ll_list_t* memPool_c::fflushData2QueueLList (const void *data, int len)
{
	ll_list_t *head = 0;
	int size = 0;
	memBbuff_t *b;

	while (len > 0){
		if (size == 0){
            b = getMembuf();
			if (head)
				LLIST_ADD_TAIL(head, &b->list);
			else
				head = &b->list;
		}
		size = memPool_c::memDataCopy(b, data, len);
		len -= size;
		data = (const char*)data + size;
	}
	return head;
}


