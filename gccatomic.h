/*
 *gcc从4.1.2提供了__sync_*系列的built-in函数，用于提供加减和逻辑运算的原子操作。
*/
#ifndef GCCATOMIC_H
#define GCCATOMIC_H
#define GCCVERSION(G,M,P) ((G)*100 + (M)*10 +P)

#ifdef QT_CORE_LIB
#include <QAtomicInt>
#define ATOMIC_GET_ADD(ptr,v) (ptr)->fetchAndAddRelaxed(v) /*返回更新前的值,然后在加value*/
#define ATOMIC_GET_SUB(ptr,v) (ptr)->fetchAndSubRelaxed(v)
#define ATOMIC_GET_OR(ptr,v)  (ptr)->fetchAndOrRelaxed(v)
#define ATOMIC_GET_AND(ptr,v) (ptr)->fetchAndAndRelaxed(v)
#define ATOMIC_GET_XOR(ptr,v) (ptr)->fetchAndXorRelaxed(v)
#define ATOMIC_GET_NAND(ptr,v)(ptr)->fetchAndNandRelaxed(v)

#define ATOMIC_ADD_GET(ptr,v) (ptr)->operator+=(v) /*先加value 返回更新后的值,*/
#define ATOMIC_SUB_GET(ptr,v) (ptr)->operator-=(v)
#define ATOMIC_OR_GET(ptr,v)  (ptr)->operator|=(v)
#define ATOMIC_AND_GET(ptr,v) (ptr)->operator&=(v)
#define ATOMIC_XOR_GET(ptr,v) (ptr)->operator^=(v)

#define ATOMIC_GET_SET(ptr,v) (ptr)->fetchAndStoreRelaxed(v) /*将*ptr设为value并返回*ptr操作之前的值。*/
#define ATOMIC_RELEASE(ptr)   (ptr)->store(0)         /*将*ptr置0*/


#elif GCCVERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__) < GCCVERSION(4,1,2)
#define __USE_MY_TEST_ATOMIC 1
extern long long   int __test_myfunc_add_atomic (void *p, int v, int w, int i);
#define ATOMIC_GET_ADD(ptr,value)  __test_myfunc_add_atomic(ptr, value, sizeof (*(ptr)), 0)
#define ATOMIC_ADD_GET(ptr,value) __test_myfunc_add_atomic(ptr, value, sizeof (*(ptr)), 1)

extern long long   int __test_myfunc_sub_atomic (void *p, int v, int w, int i);
#define ATOMIC_SUB_GET(p,v ) __test_myfunc_sub_atomic(p,v,sizeof (*(p)),1)


extern  long long int __test_myfunc_getset_atomic(void *p, int v , int w);
#define ATOMIC_GET_SET(ptr,value) __test_myfunc_getset_atomic(ptr,value,sizeof (*(ptr)))


#else
/*以下ptr所指向的类型长度为 1 2 4 8整型*/
#define ATOMIC_GET_ADD(ptr,value) __sync_fetch_and_add(ptr, value) /*返回更新前的值,然后在加value*/
#define ATOMIC_GET_SUB(ptr,value ) __sync_fetch_and_sub(ptr, value)
#define ATOMIC_GET_OR(ptr,value ) __sync_fetch_and_or(ptr, value)
#define ATOMIC_GET_AND(ptr,value ) __sync_fetch_and_and(ptr, value)
#define ATOMIC_GET_XOR(ptr,value ) __sync_fetch_and_xor(ptr, value)
#define ATOMIC_GET_NAND(ptr,value ) __sync_fetch_and_nand(ptr, value)

#define ATOMIC_ADD_GET(ptr,value ) __sync_add_and_fetch(ptr, value) /*先加value 返回更新后的值,*/
#define ATOMIC_SUB_GET(ptr,value ) __sync_sub_and_fetch(ptr, value)
#define ATOMIC_OR_GET(ptr,value ) __sync_or_and_fetch(ptr, value)
#define ATOMIC_AND_GET(ptr,value ) __sync_and_and_fetch(ptr, value)
#define ATOMIC_XOR_GET(ptr,value ) __sync_xor_and_fetch(ptr, value)

/*以下这两个函数提供原子的比较和交换，如果*ptr == oldval,就将newval写入*ptr*/
#define ATOMIC_BOOL_CMP_SWAP(ptr,oldval,newval) __sync_bool_compare_and_swap(ptr,oldval,newval) /*在相等并写入的情况下返回true*/
#define ATOMIC_VAL_CMP_SWAP(ptr,oldval,newval) __sync_val_compare_and_swap(ptr,oldval,newval) /*返回操作之前的值*/


#define ATOMIC_GET_SET(ptr,value) __sync_lock_test_and_set(ptr,value) /*将*ptr设为value并返回*ptr操作之前的值。*/
#define ATOMIC_RELEASE(ptr) __sync_lock_release(ptr) /*将*ptr置0*/
#endif

#endif // GCCATOMIC_H
