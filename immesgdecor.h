#ifndef IMMESGPARSE_H
#define IMMESGPARSE_H
#include <string.h>
#include "immessage_def.h"

class ImessageApplyNum : public ImmessageMan
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

    ImessageApplyNum(ImmessageMan *imesgDataPtr);
    ~ImessageApplyNum() {}
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

class ImessageLogon : public ImmessageMan
{
public:
    #pragma pack(1)
    typedef struct {
        char passwd[16];
        int succ;
    }LogonUsr_t;
    #pragma pack()
    ImessageLogon (ImmessageMan *m);
    void setLogonPassword (const char *pass);
    const char* getPassword() {return logonUsrPtr->passwd;}
    void setAuthSucc (int b);
    int isAuthSucced();

private:
    LogonUsr_t *logonUsrPtr;
    bool isPushed;
    void pushMemLength ();
};

////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////
class ImmesgObsev {
public:
    ImmesgObsev (Imesgtpe_t t){imtype = t;}
    virtual ~ImmesgObsev() {}
    int getMesgType () {return imtype;}
    virtual void workIngWithRecvMessage (ImmessageData &im, void *p) = 0;
private:
    Imesgtpe_t imtype;
};
#if 0

#pragma pack(1)
typedef struct {
    char name[24];
    int usrNu;
}imMeagLogon_t;
#pragma pack()

class imMesgLogon_c : public imMessageMan_c
{
public:
    imMesgLogon_c(int usrnu);
    imMesgLogon_c (imMessageMan_c &src);
    bool setUsrInfo(const char *name);
    const char* getName ();
private:

};

/////////////////////////////
/////////////////////////////
#pragma pack(1)
typedef struct {
    char name[24];
    int usrnu;
    netAddr_t remAddr;
}imMesgOnline_t;
#pragma pack()


class imMesgOnLineList_c : public imMessageMan_c
{
public:
    imMesgOnLineList_c();
    imMesgOnLineList_c (const imMessageMan_c &src);
    bool addOneUsrList (const imMesgOnline_t &l);
    const imMesgOnline_t* getListLoop ();
private:
    int contenLen;
};

/*
 * p2p 连接过程(a--->b)
 * 1 IMMESG_P2PUDP_ASK a向服务器发送IMMESG_P2PUDP_ASK请求，请求向b（dstUsrNu）进行p2p通信，a要带上自己本地端的UDP IP和端口。
 * 2 IMMESG_P2PUDP_CONNSET a进而向b的外网地址B发送P2P请求连接报文。
 * 3 IMMESG_P2PUDP_ASK 服务器向b转发a的IMMESG_P2PUDP_ASK请求，并告知a的本地UDP IP和端口。
 * 4 IMMESG_P2PUDP_CONNSET b向a的外网地址A发送IMMESG_P2PUDP_CONNSET报文。
 * 5 IMMESG_P2PUDP_CONNSET a和b互相接受对方的IMMESG_P2PUDP_CONNSET报文，并把其中的ack置1，然后把报文返回。
 * 6 IMMESG_P2PUDP_CHAT a与b进行聊天，dataText为聊天内容。
 * 7 IMMESG_P2PUDP_KEEP 使p2p通道保持连接，每隔5秒发送一次，接到该报文后，把ack置1，然后把报文返回.
*/
enum {
    IMMESG_P2PUDP_ASK,
    IMMESG_P2PUDP_CONNSET1,
    IMMESG_P2PUDP_CONNSET2,
    IMMESG_P2PUDP_CHAT,
    IMMESG_P2PUDP_KEEP,
};

#pragma pack(1)
typedef struct {
    int dstUsrNu;
    int srcUsrNu;
    //unsigned int dstaddr;
    //unsigned short dstport;
    //unsigned int srcaddr;
    //unsigned short srcport;
    int dataType;
    int ack;
    int dataLen;
    char dataText[0];
}imMesgP2Pdata_t;
#pragma pack()

class imMesgP2Pdata_c : public imMessageMan_c
{
public:
    imMesgP2Pdata_c(int dstnu, int srcnu, int t);
    imMesgP2Pdata_c(const imMessageMan_c &src);

    const imMesgP2Pdata_t& getp2pContent () {return *imMesgP2PdataPtr;}
    void setAck () ;
    bool pushChatData (const void *data = 0, int len = 0);
private:
    imMesgP2Pdata_t *imMesgP2PdataPtr;

};
#endif
#endif // IMMESGPARSE_H




























