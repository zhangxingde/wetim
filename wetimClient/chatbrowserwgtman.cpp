#include <string.h>
#include <QHostAddress>
#include "chatbrowserwgtman.h"
#include "usrchatborwerwidget.h"
#include "p2pudpchannel.h"
#include "immessagchannel.h"
#include "climesgobsev.h"
#include "mainpanel.h"

ChatBrowserWgtMan::ChatBrowserWgtMan(QWidget *parent):maxWgtNum(sizeof (frdChatWgtSet)/sizeof(frdChatWgtSet[0]))
{
    parentWgtPtr = parent;
    for (int i = 0; i < maxWgtNum; ++i){
        frdChatWgtSet[i].chatWgtSet = 0;
        frdChatWgtSet[i].dstAddr.ipv4 = 0;
        frdChatWgtSet[i].dstAddr.port = 0;
        frdChatWgtSet[i].p2pChannelId = 0;
        frdChatWgtSet[i].uid = 0;
        frdChatWgtSet[i].ims.clear();
    }

    p2pUdpChannelPtr = new P2PUdpChannel(0);
    mesgChannelPtr = ImmesageChannel::getInstance();
    mesgChannelPtr->regOneImobsever(new ImesgChatMessage, this);
}

ChatBrowserWgtMan::~ChatBrowserWgtMan()
{
    closeAllChatWgt();
    delete p2pUdpChannelPtr;
    mesgChannelPtr->close();
}

int ChatBrowserWgtMan::openChatBrowserWgtByUid(int frduid, int mineUsrId, bool isHasMessage)
{
    unsigned int i;
    UsrChatBrowserWidget *w = 0;
    int uid = frduid;

    for (i = 0; i < maxWgtNum; ++i){
        if (frdChatWgtSet[i].uid == uid && (w = frdChatWgtSet[i].chatWgtSet)){
            w->activateWindow();
            w->show();
            return -1;
        }

    }
    for (i = 0; i < maxWgtNum; ++i){
        if (isHasMessage){
            if (!frdChatWgtSet[i].chatWgtSet && frdChatWgtSet[i].uid == frduid){
                break;
            }
        }else if (!frdChatWgtSet[i].chatWgtSet)
            break;
    }
    if (i == maxWgtNum)
        return -1;
    frdChatWgtSet[i].uid = uid;
    frdChatWgtSet[i].chatWgtSet = w = new UsrChatBrowserWidget(uid,mineUsrId, parentWgtPtr);
    w->setChatMessageOutFun(chatWgtOutChatMessage, this);
    w->setWindowsNum(i);
    w->show();

    if (isHasMessage){
        int n = 0;
        for (n = 0; n < frdChatWgtSet[i].ims.size(); ++n){
            w->pushChatMessageIn(frdChatWgtSet[i].ims[n].getMesgDataPtr(),frdChatWgtSet[i].ims[n].mesgLength());
        }
        frdChatWgtSet[i].ims.clear();
    }

    if (i >= 0){
        int chid;
        chid =  p2pUdpChannelPtr->getNewChannelId(uid, mineUsrId);
        p2pUdpChannelPtr->setChannelNoticeCallbak(chid, p2pChannelNotice, this);
        setChatWgtP2PChannelid(i, chid);
        p2pUdpChannelPtr->startGetThroughP2P(chid);
     }
    return i;
}

void ChatBrowserWgtMan::closeChatBrowserWgtByUid(int uid)
{
    unsigned int i;
    UsrChatBrowserWidget *w = 0;

    for (i = 0; i < maxWgtNum; ++i){
        if (frdChatWgtSet[i].uid == uid){
            p2pUdpChannelPtr->closeChannle(frdChatWgtSet[i].p2pChannelId);
            if ((w = frdChatWgtSet[i].chatWgtSet)){
                delete w;
            }
            frdChatWgtSet[i].chatWgtSet = 0;
            frdChatWgtSet[i].dstAddr.ipv4 = 0;
            frdChatWgtSet[i].dstAddr.port = 0;
            frdChatWgtSet[i].p2pChannelId = 0;
            frdChatWgtSet[i].uid = 0;
            frdChatWgtSet[i].ims.clear();
            break;
        }
    }
}

void ChatBrowserWgtMan::setChatWgtP2PChannelid(int n, int id)
{
    frdChatWgtSet[n].p2pChannelId = id;
}
#include <QDebug>
int ChatBrowserWgtMan::recvChatMessage(const ImmessageData &data)
{
    for (int i = 0; i < maxWgtNum; ++i){
        if (frdChatWgtSet[i].uid == data.getSrcUsrId()){
            if (frdChatWgtSet[i].chatWgtSet){
                frdChatWgtSet[i].chatWgtSet->pushChatMessageIn(data.getMesgDataPtr(), data.mesgLength());
            }else{
                frdChatWgtSet[i].ims.push_back(data);
            }
            return 1;
        }
    }
    for (int i = 0; i < maxWgtNum; ++i){
        if (!frdChatWgtSet[i].chatWgtSet){
            frdChatWgtSet[i].uid = data.getSrcUsrId();
            frdChatWgtSet[i].ims.push_back(data);
            break;
        }
    }
    ((MainPanel*)parentWgtPtr)->noticeMessagComeing(data.getSrcUsrId());
    qDebug()<<"not open usr,meg="<<(const char*)data.getMesgDataPtr();
    return 0;

}

void ChatBrowserWgtMan::closeAllChatWgt()
{
    unsigned int i;

    for (i = 0; i < maxWgtNum; ++i){
        if (frdChatWgtSet[i].chatWgtSet){
            frdChatWgtSet[i].chatWgtSet->close();
            delete frdChatWgtSet[i].chatWgtSet;
            memset(frdChatWgtSet+i, 0, sizeof(frdChatWgtSet[i]));
        }
    }
}

void ChatBrowserWgtMan::p2pChannelNotice(int chid, netAddr_t dstAddr, int type, void *p)
{
    ChatBrowserWgtMan *m = (ChatBrowserWgtMan*)p;
    UsrChatBrowserWidget *w = 0;
qDebug()<<"p2p is connected";
    for (int i = 0; i < m->maxWgtNum; ++i){
        if (m->frdChatWgtSet[i].p2pChannelId ==chid){
            w = m->frdChatWgtSet[i].chatWgtSet;
            m->frdChatWgtSet[i].dstAddr = dstAddr;
            break;
        }
    }
    if (w){
        w->setFrdPeerInfo(dstAddr.ipv4, dstAddr.port);
    }
}

int ChatBrowserWgtMan::chatWgtOutChatMessage(int windown, const QString &chatMessage, void *p)
{
    ChatBrowserWgtMan *wgtManPtr = (ChatBrowserWgtMan*)p;
    UsrChatBrowserWidget *w = wgtManPtr->frdChatWgtSet[windown].chatWgtSet;
    netAddr_t &dstaddr = wgtManPtr->frdChatWgtSet[windown].dstAddr;
    QByteArray binBytes = chatMessage.toUtf8();
    ImmessageData data(IMMESG_NETDATA_IMCHAT);

    data.setDstSrcUsr(w->getFrdUid(), w->getMineUid());
    data.addMesgData(binBytes.length(), binBytes.constData());

    if (dstaddr.ipv4 && dstaddr.port){
        QHostAddress hostaddr(dstaddr.ipv4);

        return wgtManPtr->mesgChannelPtr->pushUdpDataOut(data, hostaddr.toString(), dstaddr.port);
    }
    return wgtManPtr->mesgChannelPtr->pushUdpDataOut(data);
}
