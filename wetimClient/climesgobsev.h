#ifndef CLIMESGOBSEV_H
#define CLIMESGOBSEV_H
#include "immesgdecor.h"

class ImesgApplyNumObsev : public ImmesgObsev {
public:
    ImesgApplyNumObsev();
    ~ImesgApplyNumObsev(){}
private:
    void workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p);

};

class ImesgLononObsev : public ImmesgObsev {
public:
    ImesgLononObsev():ImmesgObsev(IMMESG_USER_LOGON){}
private:
    void workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p);
};

class ImesgKeepAliveObsev : public ImmesgObsev {
public:
    ImesgKeepAliveObsev():ImmesgObsev(IMMESG_UDP_KEEPALIVE){}
private:
    void workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p);
};

class ImesgUsrOnlistObsev : public ImmesgObsev {
public:
    ImesgUsrOnlistObsev():ImmesgObsev(IMMESG_USER_ONLIST){}
private:
    void workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p);
};

class ImesgNetGetUdpAddr : public ImmesgObsev
{
public:
    ImesgNetGetUdpAddr():ImmesgObsev(IMMESG_NETGET_UDPADDR){}
private:
    void workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p);
};

class ImesgP2pUdp : public ImmesgObsev
{
public:
    ImesgP2pUdp():ImmesgObsev(IMMESG_NETP2P_UDPDATA){}
private:
    void workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p);
};

#endif // CLIMESGOBSEV_H
