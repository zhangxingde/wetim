#ifndef SEVMESGOBSEV_H
#define SEVMESGOBSEV_H
#include "immesgdecor.h"

class ImesgApplyNumObsev : public ImmesgObsev {
public:
    ImesgApplyNumObsev();
private:
    void workIngWithRecvMessage(ImmessageData &im, void *p);

};

#endif // SEVMESGOBSEV_H
