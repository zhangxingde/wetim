#ifndef SingletonTempBase_H
#define SingletonTempBase_H

#ifdef  _REENTRANT
    #include <pthread.h>
#elif defined(QT_CORE_LIB)
    #include <QMutexLocker>
#endif
template <typename T>
class SingletonTempBase
{
public:
    //SingletonTempBase();
    static T* getInstance ();
    static void close();
    virtual ~SingletonTempBase (){}
protected:
    static void mutexLock ();
    static void umutexLock ();
private:

    static T *instenPtr;
    static unsigned int num;
#ifdef  _REENTRANT
    static pthread_mutex_t mLock;
#elif defined(QT_CORE_LIB)
    static QMutex mLock;
#endif
};

template<typename T> T* SingletonTempBase<T>::instenPtr = 0;
template<typename T> unsigned int SingletonTempBase<T>::num = 0;

#if defined _REENTRANT
template<typename T> pthread_mutex_t SingletonTempBase<T>::mLock = PTHREAD_MUTEX_INITIALIZER;
#elif defined(QT_CORE_LIB)
template<typename T> QMutex SingletonTempBase<T>::mLock;
#endif

#if 0
template<typename T>
SingletonTempBase<T>::SingletonTempBase()
{

}
#endif

template<typename T>
T *SingletonTempBase<T>::getInstance()
{
    mutexLock();
    if (!instenPtr){
        instenPtr = new T;
    }
    if (instenPtr){
        ++num;
    }
    umutexLock();
    return instenPtr;
}


template<typename T>
void SingletonTempBase<T>::close()
{
    mutexLock();
    if (num > 0 && --num == 0){
        delete instenPtr;
        instenPtr = 0;
    }
    umutexLock();
}

template<typename T>
void SingletonTempBase<T>::mutexLock()
{
#if defined _REENTRANT
    pthread_mutex_lock(&mLock);
#elif defined(QT_CORE_LIB)
    mLock.lock();
#endif
}

template<typename T>
void SingletonTempBase<T>::umutexLock()
{
#if defined _REENTRANT
    pthread_mutex_unlock(&mLock);
#elif defined(QT_CORE_LIB)
    mLock.unlock();
#endif
}



#endif // SingletonTempBase_H
