#ifndef IMMESSAGE_DEF_H
#define IMMESSAGE_DEF_H
#if defined(__linux__) || defined(__linux)
    #include <arpa/inet.h>
#elif defined(QT_CORE_LIB)
    #include <QtEndian>
    #define htonl(n) qToBigEndian(n)
    #define htons(n) qToBigEndian(n)
    #define ntohl(n) qFromBigEndian(n)
    #define ntohs(n) qFromBigEndian(n)
#endif

typedef enum {
    IMMESG_NONE = 0,
    IMMESG_APPLYNUM,

    IMMESG_USER_LOGON ,
    IMMESG_USER_LOGOUT,
    IMMESG_USER_BROAD,
    IMMESG_USER_ONLIST,

    IMMESG_UDP_KEEPALIVE,

    IMMESG_NETGET_UDPADDR,
    IMMESG_NETP2P_UDPDATA,

    IMMESG_DATA_ACK,
    IMMESG_NET_CONNECT,
    __IMMESG_MAX_NUM
}Imesgtpe_t;

#define IMMESG_CHCK1 0xf0f0f0f0U
#define IMMESG_CHCK2 0x0f0f0f0fU
#define IMMESG_MESGDATA_MAXLEN 8192
#pragma pack(1)
typedef struct {
    unsigned int checkNum1; //IMMESG_CHCK1
    int dstUsr;             //目标用户号
    int srcUsr;             //发送端的用户号
    unsigned int synNum;    //消息同步号
    int mesgType;           //Imesgtpe_t
    unsigned int mesgLen;   //消息mesgData的长度
    char mesgData[1];       //消息的内容(变长,最长为IMMESG_MESGDATA_MAXLEN)
    unsigned int checkNum2; //IMMESG_CHCK2
}Immessage_t;

typedef struct {
    unsigned int ipv4;
    unsigned short port;
}netAddr_t;
#pragma pack()

class ImmessageData;
class ImmessageMan
{
public:
    ImmessageMan (ImmessageMan *origImmesgPtr);
    ImmessageMan (const ImmessageMan *origImmesgPtr);
    virtual ~ImmessageMan() {}
    virtual void setDstSrcUsr (int dst, int src);
    virtual int getDstUsrId () const;
    virtual int getSrcUsrId () const;
    virtual unsigned int  addMesgData (unsigned int len, const void *p = 0);
    virtual const void* getDataPtr () const;
    virtual unsigned int length() const;
    virtual unsigned int mesgLength() const;
    virtual const char* getMesgDataPtr () const;
protected:
    ImmessageMan *mOrigImmesgPtr;
    const ImmessageMan *mConstOrigImmesgPtr;

};

class ImmessageData : public ImmessageMan
{
public:
    ImmessageData(Imesgtpe_t t);
    ImmessageData ();
    ImmessageData (const ImmessageData &other);
    void setDstSrcUsr(int dst, int src);
    int getDstUsrId () const;
    int getSrcUsrId () const;
    unsigned int addMesgData(unsigned int len, const void *p = 0);
    const void* getDataPtr() const {return mesg.megBuf;}
    unsigned int length() const {return mLength;}
    unsigned int mesgLength() const;
    const char* getMesgDataPtr() const;

    unsigned int recvRawData(const void *p, unsigned int len);
    bool isValid () const;
    int getMesgType () const;
private:
    union {
        Immessage_t head;
        char megBuf[sizeof(Immessage_t)+IMMESG_MESGDATA_MAXLEN];
    }mesg;
    unsigned int mLength;
    char *curMesgDataPtr;

};


#if 0
class imMessageMan_c {
public:
    imMessageMan_c (int size = 0, char ch = 0, int istcp = 1);
    imMessageMan_c (int un, imMesgType_t t, int istcp = 1);
    imMessageMan_c (const void *data, int len, int istcp = 1);
    imMessageMan_c (const imMessageMan_c &src);
    imMessageMan_c& operator= (const imMessageMan_c &src);
    ~imMessageMan_c();

    bool isValid () const;
    int getDir () {return sendDir;}
    const char* getMessageContent () {return getImMesgHeadPtr()->mesgContentData;}
    char *getMemBuf () {return (char*)getImMesgHeadPtr();}
    int getMessageContentLen () {return getImMesgHeadPtr()->mesgContentLen;}
    int length () {return imMessagelen;}
    int getMesgType () const  {return getImMesgHeadPtr()->mesgType;}
    bool addMesgContent (const void *src, int len);
    int getUsrNu () const {return getImMesgHeadPtr()->usrNu;}
    void deStroyMem ();
    int isTcp () {return tcp;}
    void setTcp () {tcp = 1;}
    const netAddr_t& getNetAddr () {return addr;}
    void setAddr (const netAddr_t &n) {addr = n;}
private:
    typedef struct {
        int useCount;
        imMessage_t immesg[0];
    }imMesgRef_t;

    union {
        char *bufptr;
        imMesgRef_t *refptr;
    }unptr;

    imMessage_t imMessageHead;
    int imMessagelen;
    int memCallocSize;
    int sendDir;//发送方向 0 向外， 1 向内
    int tcp;
    netAddr_t addr;//表示消息的来源地址


    void copyFormAnother (const imMessageMan_c &src);
    const imMessage_t* getImMesgHeadPtr () const
    {
        if (unptr.bufptr){
            return unptr.refptr->immesg;
        }
        return &imMessageHead;
    }
    bool reSizeMemSize(int len);

};
#endif

#endif // IMMESSAGE_DEF_H

