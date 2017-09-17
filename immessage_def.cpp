#include <string.h>
#include <stdio.h>
#include "immessage_def.h"
#include "gccatomic.h"

ImmessageMan::ImmessageMan(ImmessageMan *origImmesgPtr)
{
    mOrigImmesgPtr = origImmesgPtr;
}

void ImmessageMan::setDstSrcUsr(int dst, int src)
{
    mOrigImmesgPtr->setDstSrcUsr(dst,src);
}

unsigned int ImmessageMan::addMesgData(const void *p, unsigned int len)
{
    return mOrigImmesgPtr->addMesgData(p,len);
}

const void* ImmessageMan::getDataPtr() const
{
    return mOrigImmesgPtr->getDataPtr();
}

unsigned int ImmessageMan::length() const
{
    return mOrigImmesgPtr->length();
}

unsigned int ImmessageMan::mesgLength() const
{
    return mOrigImmesgPtr->mesgLength();
}

const char *ImmessageMan::getMesgDataPtr() const {return mOrigImmesgPtr->getMesgDataPtr();}


ImmessageData::ImmessageData(Imesgtpe_t t):ImmessageMan(this)
{
    memset(&mesg.head, 0, sizeof (mesg.head));
    mesg.head.checkNum1 = htonl(IMMESG_CHCK1);
    mesg.head.checkNum2 = htonl(IMMESG_CHCK2);
    mesg.head.mesgType = htonl((int)t);

    mLength = sizeof(Immessage_t);
    curMesgDataPtr = mesg.head.mesgData;
}

ImmessageData::ImmessageData():ImmessageMan(this)
{
    memset(&mesg.head, 0, sizeof (mesg.head));
    mesg.head.checkNum1 = htonl(IMMESG_CHCK1);
    mesg.head.checkNum2 = htonl(IMMESG_CHCK2);

    mLength = 0;
    curMesgDataPtr = mesg.head.mesgData;
}

void ImmessageData::setDstSrcUsr(int dst, int src)
{
    mesg.head.dstUsr = htonl(dst);
    mesg.head.srcUsr = htonl(src);
}

unsigned int ImmessageData::addMesgData(const void *p, unsigned int len)
{
    if (mLength + len > sizeof(mesg.megBuf)){
        return 0;
    }
    mLength += len;
    memcpy(curMesgDataPtr, p, len);
    curMesgDataPtr += len;
    mesg.head.mesgLen = htonl(curMesgDataPtr - mesg.head.mesgData);
    *(unsigned int*)(curMesgDataPtr + 1) = htonl(IMMESG_CHCK2);
    return len;
}

unsigned int ImmessageData::mesgLength() const
{
    return ntohl(mesg.head.mesgLen);
}

const char* ImmessageData::getMesgDataPtr() const
{
    return mesg.head.mesgData;
}

unsigned int ImmessageData::recvRawData(const void *p, unsigned int len)
{
    if (mLength + len > sizeof(mesg.megBuf)){
        return 0;
    }
    memcpy(mesg.megBuf + mLength, p, len);
    mLength += len;
    return len;
}

bool ImmessageData::isValid() const
{
    if (ntohl(mesg.head.checkNum1) != IMMESG_CHCK1){
        return 0;
    }
    if (ntohl(mesg.head.mesgLen) > IMMESG_MESGDATA_MAXLEN){
        return 0;
    }
    if (*(unsigned int*)(mesg.head.mesgData + ntohl(mesg.head.mesgLen) + 1) != htonl(IMMESG_CHCK2)){
        return 0;
    }
    return 1;
}

int ImmessageData::getMesgType() const
{
    return ntohl(mesg.head.mesgType);
}




#if 0

imMessageMan_c::imMessageMan_c(int size, char ch, int istcp)
{
    if (size <= 0){
        unptr.bufptr = 0;
        imMessagelen = 0;
        memCallocSize = 0;
        memset(&imMessageHead, 0, sizeof (imMessageHead));
        return;
    }
    memCallocSize = size + sizeof (imMesgRef_t);
    unptr.bufptr = new char[memCallocSize];
    if (unptr.bufptr){
        ATOMIC_GET_SET(&unptr.refptr->useCount, 1);
        imMessagelen = size;
        memset(unptr.refptr->immesg, ch, size);
    }
    sendDir = 1;
    tcp = istcp;
}

imMessageMan_c::imMessageMan_c(int un, imMesgType_t t, int istcp)
{
    unptr.bufptr = 0;
    imMessageHead.checkNum1 = IMMESG_CHCK1;
    imMessageHead.checkNum2 = IMMESG_CHCK2;
    imMessageHead.usrNu = un;
    imMessageHead.mesgContentLen = 0;
    imMessageHead.mesgType = t;
    imMessagelen = sizeof (imMessageHead);
    sendDir = 0;
    memCallocSize = 0;
    tcp = istcp;
}

imMessageMan_c::imMessageMan_c(const void *data, int len, int istcp)
{
    memCallocSize = len + sizeof (imMesgRef_t);
    unptr.bufptr = new char[memCallocSize];
    if (unptr.bufptr){
        ATOMIC_GET_SET(&unptr.refptr->useCount, 1);
        memcpy(unptr.refptr->immesg, data, len);
        imMessagelen = len;
    }
    sendDir = 1;
    tcp = istcp;
}

imMessageMan_c::imMessageMan_c(const imMessageMan_c &src)
{
    copyFormAnother(src);
}

imMessageMan_c& imMessageMan_c::operator= (const imMessageMan_c &src)
{
    if (this == &src)
        return *this;
    if (unptr.bufptr){
        if (unptr.bufptr == src.unptr.bufptr){
            return *this;
        }else{
            deStroyMem();
        }
    }
    copyFormAnother(src);
    return *this;
}

imMessageMan_c::~imMessageMan_c()
{
   deStroyMem();
}

void imMessageMan_c::deStroyMem()
{
    if (unptr.bufptr && (ATOMIC_SUB_GET(&unptr.refptr->useCount,1) == 0)){
        delete[] unptr.bufptr;
    }
    unptr.bufptr = 0;
}

bool imMessageMan_c::addMesgContent(const void *src, int len)
{
    const unsigned int chk2 = IMMESG_CHCK2;

    if (!src || len <= 0)
        return 0;
    if (!reSizeMemSize(len))
        return 0;
    memcpy(unptr.refptr->immesg[0].mesgContentData + unptr.refptr->immesg[0].mesgContentLen,
            src, len);
    unptr.refptr->immesg[0].mesgContentLen += len;
    memcpy(unptr.refptr->immesg[0].mesgContentData + unptr.refptr->immesg[0].mesgContentLen,
            &chk2, sizeof (chk2));
    imMessagelen += len;
    return 1;
}

void imMessageMan_c::copyFormAnother(const imMessageMan_c &src)
{
    if (src.unptr.bufptr)
        ATOMIC_ADD_GET(&src.unptr.refptr->useCount,1);
    imMessageHead = src.imMessageHead;
    unptr = src.unptr;
    imMessagelen = src.imMessagelen;
    sendDir = src.sendDir;
    memCallocSize = src.memCallocSize;
    tcp = src.tcp;
    addr = src.addr;
}

bool imMessageMan_c::reSizeMemSize(int len)//大小改变和原来数据的拷贝 以及脱离原来的共享指针
{
    char *p = unptr.bufptr, *p2;

    if ((len + imMessagelen + sizeof (imMesgRef_t) <= memCallocSize) &&
            (p && unptr.refptr->useCount == 1)){//内存足够其该内存未被被其他对象引用
        return 1;
    }
    memCallocSize = sizeof (imMesgRef_t) +((imMessagelen + len) << 1);//ref头 + （预计现有消息的2被增长）
    p2 = new char[memCallocSize];
    if (!p2){
        unptr.bufptr = p;
        return 0;
    }
    if (p){//已经进行了第n次内存分配
        memcpy(p2, p, sizeof (imMesgRef_t)+ imMessagelen);
        deStroyMem();
        unptr.bufptr = p2;
    }else{//第一次进行内存分配
        unptr.bufptr = p2;
        unptr.refptr->immesg[0] = imMessageHead;
        unptr.refptr->useCount = 1;
        unptr.refptr->immesg[0].mesgContentLen = 0;
    }
    unptr.refptr->useCount = 1;//当前对象引用
    return 1;
}

bool imMessageMan_c::isValid() const
{
    const imMessage_t *imMessageHeadPtr = getImMesgHeadPtr();

    if (imMessagelen != imMessageHeadPtr->mesgContentLen + sizeof (imMessage_t)){
        return 0;
    }
    unsigned int *chk2ptr = (unsigned int*)(imMessageHeadPtr->mesgContentData +
            imMessageHeadPtr->mesgContentLen);
    if ((imMessageHeadPtr->checkNum1 != IMMESG_CHCK1) ||
            (*chk2ptr != IMMESG_CHCK2)){
       return 0;
    }

    return 1;
}
#endif
