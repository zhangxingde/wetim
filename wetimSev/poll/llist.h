#ifndef DOUBLELINK_H
#define DOUBLELINK_H
#include <stddef.h>
typedef struct ll_list_t{
    struct ll_list_t *pre;
    struct ll_list_t *next;
}ll_list_t;

#define MEMBER_ENTRY(ptr, type, member) ((type*)((char*)(ptr) - offsetof(type, member)))
#define MEMBER_POINTER(ptr,type,member) ((const char*)(ptr) - (offsetof(type, ptr)) - (offsetof(type, member)))

#define LLIST_INIT(n) \
do{ \
    (n)->next = n; \
    (n)->pre = n;\
}while (0)

#define LLIST_INIT2(n) {n,n}
#define LLIST_EMPTY(n) ((n) == (n)->next)
/*
 *LLIST_ADD_TAIL(front,add):
 *add加在以front为链表头的链表的结尾
*/
#define LLIST_ADD_TAIL(front,add)    \
do { \
    LLIST_INIT(add);\
    ((front)->pre)->next = (add);\
    (add)->pre = ((front)->pre); \
    (add)->next = (front); \
    (front)->pre = (add); \
}while(0)

/*
 *	LLIST_ADD_NEXT(H, N)：
 *N加在以H为链表头的链表的头的下一个
*/
#define	LLIST_ADD_NEXT(H, N)							\
	do {								\
		LLIST_INIT(N);					\
        ((H)->next)->pre = (N);				\
		(N)->next = ((H)->next);				\
        (N)->pre = (H);					\
		(H)->next = (N);					\
	} while (0)

#define LLIST_DEL(del) \
do { \
    ((del)->next)->pre = ((del)->pre);\
    ((del)->pre)->next = ((del)->next);\
    LLIST_INIT(del);\
}while (0)

#define LLIST_ADDLINK(dsth,srch) \
do {\
    (dsth)->next->pre = (srch)->pre; \
    (srch)->pre->next = (dsth)->next; \
    (srch)->pre = (dsth); \
    (dsth)->next = (srch); \
}while (0)

#define INVALIDPTR ((void*)-1)
#endif // DOUBLELINK_H
