#include <time.h>
#include "clockthreadman.h"

ClockThreadMan::ClockThreadMan()
{
    isQuit = 0;
    LLIST_INIT(&clockQueue);
    start();
}

ClockThreadMan::~ClockThreadMan()
{
    mLock.lock();
    isQuit = 1;
    wCondition.wakeOne();
    mLock.unlock();
    wait();
}

int ClockThreadMan::addClocker(clocker_list *c)
{
    if (!LLIST_EMPTY(&c->list))
        return 0;
    c->remainexps = c->expires;
    mLock.lock();
    LLIST_ADD_TAIL(&clockQueue, &c->list);
    mLock.unlock();
    return 1;
}

int ClockThreadMan::modClocker(clocker_list *c, unsigned long expires)
{
    delCloker(c);
    c->expires = expires;
    return addClocker(c);
}

void ClockThreadMan::delCloker(clocker_list *c)
{
    mLock.lock();
    if (!LLIST_EMPTY(&c->list)){
        LLIST_DEL(&c->list);
    }
    mLock.unlock();
    LLIST_INIT(&c->list);
}

void ClockThreadMan::run()
{
    ll_list_t *l;
    clocker_list *c;
    clock_t t1, diff = 0;

    while (!isQuit){
        mLock.lock();
        wCondition.wait(&mLock, 1);
        t1 = clock();
        if (isQuit){
            mLock.unlock();
            return;
        }
        l = clockQueue.next;
        while (l != &clockQueue){
            c = MEMBER_ENTRY(l, clocker_list, list);
            if ((c->remainexps - diff - 1) <= 0){
                c->fun(c->usrArg);
                c->remainexps = c->expires;
            }else{
                c->remainexps -= diff + 1;
            }
            l = l->next;
        }
        diff = clock() - t1;
        mLock.unlock();
     }
}
