#ifndef IMMESSAGEQUEUE_H
#define IMMESSAGEQUEUE_H

#include <QMutexLocker>
#include <QSemaphore>
#include <QString>
#include "immessage_def.h"


class imMessageQueue_c {
public:
    typedef struct {
        QString addr;
        int port;
        int dir;
        int isTcp;
        ImmessageData m;
    }MessageCell_t;

   imMessageQueue_c(int size);
   ~imMessageQueue_c();

   bool pushMesg (const ImmessageData &src, const QString &addr, int port, int dir, int isTcp);
   bool getMesg (ImmessageData &dst, QString &addr, int &port, int &dir, int &isTcp);
private:
   const int maxQueueSize;
   MessageCell_t *mesgeQueuePtr;
   QMutex mutexFree, mutexUse;
   QSemaphore semFree, semUse;
   int indexFree, indexUse;
};

#endif // IMMESSAGEQUEUE_H

