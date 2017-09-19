#ifndef CLIMESGOBSEV_H
#define CLIMESGOBSEV_H
#include "immesgdecor.h"

class ImesgApplyNumObsev : public ImmesgObsev {
public:
    ImesgApplyNumObsev();
    ~ImesgApplyNumObsev(){}
private:
    void workIngWithRecvMessage(ImmessageData &im, void *p);

};

class ImesgLononObsev : public ImmesgObsev {
public:
    ImesgLononObsev():ImmesgObsev(IMMESG_USER_LOGON){}
private:
    void workIngWithRecvMessage(ImmessageData &im, void *p);
};

#endif // CLIMESGOBSEV_H
