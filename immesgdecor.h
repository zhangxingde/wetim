#ifndef IMMESGPARSE_H
#define IMMESGPARSE_H
#include <string.h>
#include "immessage_def.h"

class ImmesgDecorApplyNum : public ImmessageMan
{
public:
    #pragma pack(1)
    typedef struct {
        int uid;
        char name[32];
        char passwd[16];
        int avicon;
    }ApplyUsrInfos_t;
    #pragma pack()

    ImmesgDecorApplyNum(ImmessageMan *imesgDataPtr);
    ImmesgDecorApplyNum(const ImmessageMan *imesgDataPtr);
    ~ImmesgDecorApplyNum() {}
    void setUserName (const char *name){if (name) strncpy(usrinfoptr->name, name, sizeof(usrinfoptr->name));}
    void setUserPasswd (const char *pass) {if (pass) strncpy(usrinfoptr->passwd, pass, sizeof(usrinfoptr->passwd));}
    void setUid (int id) {usrinfoptr->uid = id;}
    void setAvicon (int i) {usrinfoptr->avicon = i;}

    const char* getName () {return usrinfoptr->name;}
    const char* getPasswd () {return usrinfoptr->passwd;}
    int getUid () {return usrinfoptr->uid;}
    int getAvicon () {return usrinfoptr->avicon;}

#ifdef SYS_SERVER
    int applyUserNumFromSev();
#endif
private:
    ApplyUsrInfos_t *usrinfoptr;
};

class ImmesgDecorLogon : public ImmessageMan
{
public:
    #pragma pack(1)
    typedef struct {
        char passwd[16];
        int succ;
    }LogonUsr_t;
    #pragma pack()
    ImmesgDecorLogon (ImmessageMan *m);
    ImmesgDecorLogon (const ImmessageMan *m);
    void setLogonPassword (const char *pass);
    const char* getPassword() {return logonUsrPtr->passwd;}
    void setAuthSucc (int b);
    int isAuthSucced();

private:
    LogonUsr_t *logonUsrPtr;
    bool isPushed;
    void pushMemLength ();
};

class ImmesgDecorOnlist : public ImmessageMan
{
public:
    #pragma pack(1)
    typedef struct {
        int n;
        int hadMore;
    }OnlistCount_t;

    typedef struct {
        int uid;
        char name[32];
        int avicon;
    }OnlistUsr_t;
    #pragma pack()

    ImmesgDecorOnlist (ImmessageMan *m);
    ImmesgDecorOnlist (const ImmessageMan *m);
    bool addOneUsr (int uid, const char *name, int avicon);
    const OnlistUsr_t* getUsrListHead () const {return onlistUsrPtr;}
    int getUsrCount() const {return ntohl(onlistCountPtr->n);}
    //void setUsrCount (int n) {onlistCountPtr->n = htonl(n);}//仅仅用于客服端的再次请求数据
    //void setHadMore (bool b);
    //bool isHadMore () const {return ntohl(onlistCountPtr->hadMore);}
private:
    OnlistCount_t *onlistCountPtr;
    OnlistUsr_t *onlistUsrPtr;

    void addCount ();
};

class ImmesgDecorUdpKeep : public ImmessageMan
{
public:
    #pragma pack(1)
    typedef struct {
        unsigned int oldIpv4;
        unsigned short oldPort;
    }RemUdpAddr_t;
    #pragma pack()

    ImmesgDecorUdpKeep (ImmessageMan *m);
    ImmesgDecorUdpKeep (const ImmessageMan *m);

    void setUdpAdddr (unsigned int ip, unsigned short port)
    {
        addMesgData(sizeof(RemUdpAddr_t));
        udpaddrPtr->oldIpv4 = ip;
        udpaddrPtr->oldPort = port;
    }

    unsigned int getIpAddr () {return udpaddrPtr->oldIpv4;}
    unsigned short getPort () {return udpaddrPtr->oldPort;}

private:
    RemUdpAddr_t *udpaddrPtr;
};

class ImmesgDecorP2PUdpData : public ImmessageMan
{
public:
    enum {
        UDP_P2P_NONE,
        UDP_P2P_REQUEST,
        UDP_P2P_CONNECT1,
        UDP_P2P_CONNECT2,
        UDP_P2P_KEEP,
        UDP_P2P_CLOSE

    };
    #pragma pack(1)
    typedef struct {
        netAddr_t rem;
        netAddr_t loc;
        int srcChid;
        int dstChid;
        char state;
        char ack;
    }p2pTransData_t;
    #pragma pack()

    ImmesgDecorP2PUdpData (ImmessageMan *m);
    ImmesgDecorP2PUdpData (const ImmessageMan *m);

    void setSrcUsrRemUdpAddr (unsigned int ipv4, unsigned short port);
    void setSrcUsrLocUdpAddr (unsigned int ipv4, unsigned short port);
    netAddr_t getSrcUsrRemUdpAddr ()
    {
        netAddr_t rem = {ntohl(p2pTransDataPtr->rem.ipv4), ntohs(p2pTransDataPtr->rem.port)};
        return rem;
    }
    netAddr_t getSrcUsrLocUdpAddr ()
    {
        netAddr_t loc = {ntohl(p2pTransDataPtr->loc.ipv4), ntohs(p2pTransDataPtr->loc.port)};
        return loc;
    }
    void setAck (bool a) {p2pTransDataPtr->ack = a;}
    void setState (char s) {p2pTransDataPtr->state = s;}
    void setDstChid (int id) {p2pTransDataPtr->dstChid = htonl(id);}
    void setSrcChid (int id) {p2pTransDataPtr->srcChid = htonl(id);}

    bool isAck () const {return p2pTransDataPtr->ack;}
    char getState () const {return p2pTransDataPtr->state;}
    const char* getUsrData () {return (const char*)(p2pTransDataPtr + 1);}
    int getUsrDataLen () {return mesgLength() - sizeof(p2pTransData_t);}
    int getSrcChid () const {return ntohl(p2pTransDataPtr->srcChid);}
    int getDstChid () const {return ntohl(p2pTransDataPtr->dstChid);}
private:
    p2pTransData_t *p2pTransDataPtr;

};


////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

#endif // IMMESGPARSE_H




























