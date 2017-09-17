#ifndef MEMPOOL_H
#define MEMPOOL_H
#include "llist.h"

#pragma pack(1) 
typedef struct memBuff{
    ll_list_t list;
    int id;
    char *memTailPtr;//buf空间的最底部
    char *dataEndPtr;//buf中数据的终点
    char *dataBeginPtr;//buf中数据的起点
	char s[1];
}memBbuff_t;
#pragma pack() 

class memPool_c {
public:

    memPool_c(int eachSize, int totalNum);
    ~memPool_c();

    memBbuff_t* getMembuf ();
    int  getAlistMembufBySzie (ll_list_t *head, int size);
    void freeMembuf (memBbuff_t*p);
	void freeMemList (ll_list_t *list);
	void freeAnyBuf (void *p);
	int getTotalMemBuffNum ();

    static int memDataCopy (memBbuff_t *dst, const void *src, int len);
	static int getDataBlockFromMemBuffList (const ll_list_t *head, const void **data);//循环调用，每次返回新的数据地址*data和其指向的长度，当返回长度为0时，
																				//循环调用结束
    /*
     * fflushData2QueueLList(const void *data, int len)
     * 把指向data地址、长度为len的数据写入到一个双向链表，并返回该双向链表的头
     *
    */
    ll_list_t *fflushData2QueueLList(const void *data, int len);
private:
    char *pbuf, **ppbuf;
    ll_list_t memBufList;
    int eachBufSize;

};

#endif // MEMPOOL_H

