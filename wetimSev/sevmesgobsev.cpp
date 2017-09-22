#include "sevmesgobsev.h"
#include "include_h/stdebug.h"

ImesgApplyNumObsev::ImesgApplyNumObsev():ImmesgObsev(IMMESG_APPLYNUM)
{

}

void ImesgApplyNumObsev::workIngWithRecvMessage(ImmessageData &im, void *p)
{
    ImmesgDecorApplyNum m(&im);
    STD_DEBUG("pppp");
}
