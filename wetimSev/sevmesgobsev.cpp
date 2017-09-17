#include "sevmesgobsev.h"
#include "include_h/stdebug.h"

ImesgApplyNumObsev::ImesgApplyNumObsev():ImmesgObsev(IMMESG_APPLYNUM)
{

}

void ImesgApplyNumObsev::workIngWithRecvMessage(ImmessageData &im, void *p)
{
    ImessageApplyNum m(&im);
    STD_DEBUG("pppp");
}
