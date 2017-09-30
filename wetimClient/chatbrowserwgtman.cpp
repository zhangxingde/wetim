#include <string.h>
#include "chatbrowserwgtman.h"
#include "usrchatborwerwidget.h"

ChatBrowserWgtMan::ChatBrowserWgtMan(QWidget *parent)
{
    parentWgtPtr = parent;
    memset(frdChatWgtSet, 0, sizeof(frdChatWgtSet));
}

ChatBrowserWgtMan::~ChatBrowserWgtMan()
{
    closeAllChatWgt();
}

int ChatBrowserWgtMan::openChatBrowserWgtByUid(int uid)
{
    const unsigned int maxi = sizeof (frdChatWgtSet)/sizeof(frdChatWgtSet[0]);
    unsigned int i;
    UsrChatBrowserWidget *w = 0;

    for (i = 0; i < maxi; ++i){
        if (frdChatWgtSet[i].uid == uid && (w = frdChatWgtSet[i].chatWgtSet)){
            w->activateWindow();
            w->show();
            return -1;
        }

    }
    for (i = 0; i < maxi; ++i){
        if (!frdChatWgtSet[i].chatWgtSet)
            break;
    }
    if (i == maxi)
        return -1;
    frdChatWgtSet[i].uid = uid;
    frdChatWgtSet[i].chatWgtSet = w = new UsrChatBrowserWidget(uid, parentWgtPtr);
    w->show();
    return i;
}

void ChatBrowserWgtMan::closeChatBrowserWgtByUid(int uid)
{
    const unsigned int maxi = sizeof (frdChatWgtSet)/sizeof(frdChatWgtSet[0]);
    unsigned int i;
    UsrChatBrowserWidget *w = 0;

    for (i = 0; i < maxi; ++i){
        if (frdChatWgtSet[i].uid == uid){
            if ((w = frdChatWgtSet[i].chatWgtSet)){
                delete w;
            }
            frdChatWgtSet[i].chatWgtSet = 0;
            frdChatWgtSet[i].uid = 0;
            break;
        }
    }
}

void ChatBrowserWgtMan::closeAllChatWgt()
{
    const unsigned int maxi = sizeof (frdChatWgtSet)/sizeof(frdChatWgtSet[0]);
    unsigned int i;

    for (i = 0; i < maxi; ++i){
        if (frdChatWgtSet[i].chatWgtSet){
            frdChatWgtSet[i].chatWgtSet->close();
            delete frdChatWgtSet[i].chatWgtSet;
            frdChatWgtSet[i].chatWgtSet = 0;
            frdChatWgtSet[i].uid = 0;
        }
    }
}
