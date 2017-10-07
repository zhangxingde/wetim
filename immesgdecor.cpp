#include <time.h>
#include "immesgdecor.h"
#include <string.h>

ImmesgDecorApplyNum::ImmesgDecorApplyNum(ImmessageMan *imesgDataPtr):ImmessageMan(imesgDataPtr)
{
    usrinfoptr = (ApplyUsrInfos_t*)getMesgDataPtr();
}

ImmesgDecorApplyNum::ImmesgDecorApplyNum(const ImmessageMan *imesgDataPtr):ImmessageMan(imesgDataPtr)
{
    usrinfoptr = (ApplyUsrInfos_t*)getMesgDataPtr();
}


#ifdef SYS_SERVER
int ImmesgDecorApplyNum::applyUserNumFromSev()
{
    return time(0);
}
#endif

ImmesgDecorLogon::ImmesgDecorLogon(ImmessageMan *m):ImmessageMan(m)
{
    logonUsrPtr = (LogonUsr_t*)getMesgDataPtr();
    isPushed = 0;
    //logonUsrPtr->passwd[0] = 0;
    //logonUsrPtr->succ = 0;
}

ImmesgDecorLogon::ImmesgDecorLogon(const ImmessageMan *m):ImmessageMan(m)
{
    logonUsrPtr = (LogonUsr_t*)getMesgDataPtr();
    isPushed = 0;
}

void ImmesgDecorLogon::setLogonPassword(const char *pass)
{
    if (pass){
        pushMemLength();
        strncpy(logonUsrPtr->passwd, pass, sizeof(logonUsrPtr->passwd));
    }
}

void ImmesgDecorLogon::setAuthSucc(int b)
{
    pushMemLength();
    logonUsrPtr->succ = htonl(b);
}

int ImmesgDecorLogon::isAuthSucced()
{
    return ntohl(logonUsrPtr->succ);
}

void ImmesgDecorLogon::pushMemLength()
{
    if (!isPushed){
        isPushed = 1;
        addMesgData(sizeof(LogonUsr_t));
    }
}




ImmesgDecorOnlist::ImmesgDecorOnlist(ImmessageMan *m):ImmessageMan(m)
{
    onlistCountPtr = (OnlistCount_t*)getMesgDataPtr();
    onlistUsrPtr = (OnlistUsr_t*)(onlistCountPtr + 1);

    onlistCountPtr->n = 0;
    onlistCountPtr->hadMore = 0;

    addMesgData(sizeof(*onlistCountPtr));
}

ImmesgDecorOnlist::ImmesgDecorOnlist(const ImmessageMan *m):ImmessageMan(m)
{
    onlistCountPtr = (OnlistCount_t*)getMesgDataPtr();
    onlistUsrPtr = (OnlistUsr_t*)(onlistCountPtr + 1);
}

bool ImmesgDecorOnlist::addOneUsr(int uid, const char *name, int avicon)
{
    if (!addMesgData(sizeof(OnlistUsr_t))){
        return 0;
    }
    onlistUsrPtr->uid = htonl(uid);
    onlistUsrPtr->avicon = htonl(avicon);
    strncpy(onlistUsrPtr->name, name, sizeof(onlistUsrPtr->name));
    ++onlistUsrPtr;
    addCount();
    return 1;
}

#if 0
void ImmesgDecorOnlist::setHadMore(bool b)
{
    int h = b;

    onlistCountPtr->hadMore = htonl(h);
}
#endif

void ImmesgDecorOnlist::addCount()
{
    onlistCountPtr->n = htonl(getUsrCount() + 1);
}



ImmesgDecorUdpKeep::ImmesgDecorUdpKeep(const ImmessageMan *m):ImmessageMan(m)
{
    udpaddrPtr = (RemUdpAddr_t*)getMesgDataPtr();
}

ImmesgDecorUdpKeep::ImmesgDecorUdpKeep(ImmessageMan *m):ImmessageMan(m)
{
    udpaddrPtr = (RemUdpAddr_t*)getMesgDataPtr();
}



ImmesgDecorP2PUdpData::ImmesgDecorP2PUdpData(ImmessageMan *m):ImmessageMan(m)
{
    addMesgData(sizeof(p2pTransData_t));
    p2pTransDataPtr = (p2pTransData_t*)getMesgDataPtr();
}
ImmesgDecorP2PUdpData::ImmesgDecorP2PUdpData(const ImmessageMan *m):ImmessageMan(m)
{
    p2pTransDataPtr = (p2pTransData_t*)getMesgDataPtr();
}
void ImmesgDecorP2PUdpData::setSrcUsrLocUdpAddr(unsigned int ipv4, unsigned short port)
{
    p2pTransDataPtr->loc.ipv4 = htonl(ipv4);
    p2pTransDataPtr->loc.port = htons(port);
}
void ImmesgDecorP2PUdpData::setSrcUsrRemUdpAddr(unsigned int ipv4, unsigned short port)
{
    p2pTransDataPtr->rem.ipv4 = htonl(ipv4);
    p2pTransDataPtr->rem.port = htons(port);
}






























