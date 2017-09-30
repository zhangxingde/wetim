#include "climesgobsev.h"
#include "logonui.h"
#include "mainpanel.h"
#include <QDebug>

ImesgApplyNumObsev::ImesgApplyNumObsev():ImmesgObsev(IMMESG_APPLYNUM)
{

}

void ImesgApplyNumObsev::workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p)
{
    ImmesgDecorApplyNum m(&im);


    ((LogonUi*)p)->setApplyNumInfo(m.getName(), m.getPasswd(), ntohl(m.getUid()), ntohl(m.getAvicon()));
}

//////////////////////////
void ImesgLononObsev::workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p)
{
    ImmesgDecorLogon m(&im);
    ((LogonUi*)p)->setLogonAuthState((m.isAuthSucced()));
}

void ImesgKeepAliveObsev::workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p)
{
    ((MainPanel*)p)->imMesgRecvKeepAlive(im);
}

void ImesgUsrOnlistObsev::workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p)
{
    MainPanel *panelPtr = (MainPanel*)p;
    ImmesgDecorOnlist onlist(&im);
    const ImmesgDecorOnlist::OnlistUsr_t *head = onlist.getUsrListHead();
    int n = onlist.getUsrCount();

    for (int i = 0; i < n; ++i){
        panelPtr->imMesgPutUsr2UsrListPanel(ntohl(head->uid), head->name, ntohl(head->avicon));
        ++head;
    }

}

void ImesgNetGetUdpAddr::workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p)
{
    MainPanel *panelPtr = (MainPanel*)p;

    panelPtr->imMesgGetUdpAddr(im);
}


void ImesgP2pUdp::workIngWithRecvMessage(const ImmessageData &im, const char *addr, int port, void *p)
{
    MainPanel *panelPtr = (MainPanel*)p;

    qDebug()<<"p2p recv from "<<addr<<":"<<port;
    panelPtr->imMesgP2pUdp(im);
}
