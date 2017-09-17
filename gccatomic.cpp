#include "gccatomic.h"

#ifdef __USE_MY_TEST_ATOMIC
static pthread_mutex_t  __atomicMutex = PTHREAD_MUTEX_INITIALIZER;
typedef union {
    void *p;
    char *s;
    short *h;
    int *n;
    long int *l;
    long long int *ll;
}uptrs_t;

static  void atomic_mutex_lock ()
{
    pthread_mutex_lock(& __atomicMutex);
}

static void atomic_mutex_unlock()
{
    pthread_mutex_unlock(&__atomicMutex);
}

static long long  int  __test_atomic_add_char (uptrs_t u, int v, long long *x)
{
    *x = *u.s;
    *u.s += v;
    return *u.s;
}

static long long  int  __test_atomic_sub_char (uptrs_t u, int v, long long *x)
{
    *x = *u.s;
    *u.s -= v;
    return *u.s;
}

static long long  int  __test_atomic_add_short (uptrs_t u, int v, long long *x)
{
    *x = *u.h;
    *u.h+= v;
    return *u.h;
}

static long long  int  __test_atomic_sub_short (uptrs_t u, int v, long long *x)
{
    *x = *u.h;
    *u.h-= v;
    return *u.h;
}

long long   int __test_myfunc_add_atomic (void *p, int v, int w, int i)
{
    long long int x1, x2;
    uptrs_t u ={p};

    atomic_mutex_lock();
    switch (w){
        case 1 :  x2  =__test_atomic_add_char(u,v,&x1); break;
        case 2 :  x2  =__test_atomic_add_char(u,v,&x1); break;
    }
    atomic_mutex_unlock();
    if (!i){
       return x1;
    }
    return x2;
}

long long   int __test_myfunc_sub_atomic (void *p, int v, int w, int i)
{
    long long int x1, x2;
    uptrs_t u ={p};

    atomic_mutex_lock();
    switch (w){
        case 1 :  x2  =__test_atomic_sub_char(u,v,&x1); break;
        case 2 :  x2  =__test_atomic_sub_short(u,v,&x1); break;
    }
    atomic_mutex_unlock();
    if (!i){
       return x1;
    }
    return x2;
}



long long int  __test_myfunc_getset_atomic(void *p, int v , int w)
{
    long long int x1;
    uptrs_t u ={p};

    atomic_mutex_lock();
    switch (w){
        case 1 :  x1 =*u.s; *u.s = v; break;
        case 2 :  x1 = *u.h; *u.h = v; break;
    }
    atomic_mutex_unlock();
    return  x1;
}

#endif
