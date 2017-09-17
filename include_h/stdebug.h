#ifndef __STDEBUG_H
#define __STDEBUG_H
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#define STD_DEBUG(format, ...) printf("%s (pthid = %lu, line = %d):"format"\n",__FILE__,pthread_self(),__LINE__,##__VA_ARGS__);\
    fflush(stdout)

#if 0
#ifdef CC_DEBUG
#define STD_DEBUG(format, ...) printf("%s : %s(%d):"format"",__FILE__,__func__,__LINE__,##__VA_ARGS__);\
    fflush(stdout)
#else
#define STD_DEBUG(format, ...)
#endif
#endif
#endif
