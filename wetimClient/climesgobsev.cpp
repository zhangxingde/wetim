#include "climesgobsev.h"
#include "logonui.h"
#include <QDebug>

ImesgApplyNumObsev::ImesgApplyNumObsev():ImmesgObsev(IMMESG_APPLYNUM)
{

}

void ImesgApplyNumObsev::workIngWithRecvMessage(ImmessageData &im, void *p)
{
    ImessageApplyNum m(&im);


    ((LogonUi*)p)->setApplyNumInfo(m.getName(), m.getPasswd(), ntohl(m.getUid()), ntohl(m.getAvicon()));
}
