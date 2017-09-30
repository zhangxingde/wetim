#ifndef CLOCKTHREADMAN_H
#define CLOCKTHREADMAN_H
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

#include "include_h/singletontemp.h"
#include "include_h/llist.h"

class ClockThreadMan : public SingletonTempBase <ClockThreadMan>, public QThread
{
    friend class SingletonTempBase <ClockThreadMan>;

public:
    typedef struct {
        ll_list_t list;
        unsigned long expires;
        unsigned long remainexps;
        void (*fun) (void *usrArg);
        void *usrArg;
    }clocker_list;

    static void ClockerInit (clocker_list *c, unsigned long expires, void (*fun) (void *usrArg), void *p)
    {
        memset(c, 0, sizeof(clocker_list));
        LLIST_INIT(&c->list);
        c->expires = expires;
        c->fun = fun;
        c->usrArg = p;
    }

    int addClocker (clocker_list *c);
    int modClocker (clocker_list *c, unsigned long expires);
    void delCloker (clocker_list *c);
private:
    ClockThreadMan();
    ~ClockThreadMan();

    QMutex mLock;
    QWaitCondition wCondition;
    int isQuit;
    ll_list_t clockQueue;

    void run ();
};

#endif // CLOCKTHREADMAN_H
