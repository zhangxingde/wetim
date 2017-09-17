#include <time.h>
#include "immesgdecor.h"
#include <string.h>

ImessageApplyNum::ImessageApplyNum(ImmessageMan *imesgDataPtr):ImmessageMan(imesgDataPtr)
{
    usrinfoptr = (ApplyUsrInfos_t*)getMesgDataPtr();
}

#ifdef SYS_SERVER
int ImessageApplyNum::applyUserNumFromSev()
{
    return time(0);
}
#endif

////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////



#if 0
imMesgPublish_c::imMesgPublish_c()
{
    memset(imObsevers, 0, sizeof (imObsevers));
    memset(imObsevArg, 0, sizeof (imObsevArg));
}

bool imMesgPublish_c::addOneImobsever(imMesgObsev_c *obsever, void *p)
{
    int t = obsever->getImesgType();

    if (!imObsevers[t]){
        imObsevers[t] = obsever;
        imObsevArg[t] = p;
        return 1;
    }
    return 0;
}

bool imMesgPublish_c::publishAnImmessage(const imMessageMan_c &src)
{
    int t;

    if (!src.isValid()){
        return 0;
    }
    t = src.getMesgType();
    if (imObsevers[t]){
        imObsevers[t]->workIngWithRecvMessage(src, imObsevArg[t]);
    }
    return 1;
}


imMesgLogon_c::imMesgLogon_c(int usrnu):
    imMessageMan_c(usrnu,IMMESG_USER_LOGON)
{

}

imMesgLogon_c::imMesgLogon_c(imMessageMan_c &src):
    imMessageMan_c(src)
{

}

bool imMesgLogon_c::setUsrInfo(const char *name)
{
    imMeagLogon_t imMeagLogon;

    strncpy(imMeagLogon.name, name, sizeof (imMeagLogon.name));
    return addMesgContent(&imMeagLogon, sizeof (imMeagLogon));
}

const char* imMesgLogon_c::getName()
{
    return ((imMeagLogon_t*)getMessageContent())->name;
}

/////////////////////////////
/////////////////////////////
imMesgOnLineList_c::imMesgOnLineList_c():
    imMessageMan_c(0, IMMESG_USER_ONLIST)
{
    contenLen = 0;
}

imMesgOnLineList_c::imMesgOnLineList_c(const imMessageMan_c &src):
    imMessageMan_c(src)
{
    contenLen = 0;
}

bool imMesgOnLineList_c::addOneUsrList(const imMesgOnline_t &l)
{
    return addMesgContent(&l, sizeof (l));
}

const imMesgOnline_t* imMesgOnLineList_c::getListLoop()
{
    imMesgOnline_t *p = NULL;

    if (contenLen < getMessageContentLen()){
        p = (imMesgOnline_t*)(getMessageContent() + contenLen);
        contenLen += sizeof (*p);
    }
    return p;
}

/////////////////////////////
/////////////////////////////
imMesgP2Pdata_c::imMesgP2Pdata_c(int dstnu, int srcnu, int t):
    imMessageMan_c(dstnu, IMMESG_DATA_P2PUDP, 0)
{
    imMesgP2Pdata_t m;

    memset(&m,0, sizeof (m));
    m.dstUsrNu = dstnu;
    m.srcUsrNu = srcnu;
    m.dataLen = 0;
    m.dataType = t;
    addMesgContent(&m, sizeof (m));
    imMesgP2PdataPtr = (imMesgP2Pdata_t*)getMessageContent();
}

imMesgP2Pdata_c::imMesgP2Pdata_c(const imMessageMan_c &src):
    imMessageMan_c(src)
{
    imMesgP2PdataPtr = (imMesgP2Pdata_t*) getMessageContent();
}

void imMesgP2Pdata_c::setAck()
{
    imMesgP2PdataPtr = (imMesgP2Pdata_t*) getMessageContent();
    imMesgP2PdataPtr->ack = 1;
}

bool imMesgP2Pdata_c::pushChatData(const void *data, int len)
{
    bool b;

    if (!data){
        return 0;
    }
    b = addMesgContent(data, len);
    if (b){
        imMesgP2PdataPtr = (imMesgP2Pdata_t*) getMessageContent();//内存可能被重新申请
        imMesgP2PdataPtr->dataLen += len;
    }
    return b;
}

#endif


























