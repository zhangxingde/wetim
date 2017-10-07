#ifndef CHATBROWSERWGTMAN_H
#define CHATBROWSERWGTMAN_H
#include <vector>
#include "immessage_def.h"

class QWidget;
class QString;

class UsrChatBrowserWidget;
class P2PUdpChannel;
class ImmesageChannel;

class ChatBrowserWgtMan
{
public:
    typedef std::vector<ImmessageData> UnReadImmesgVec;

    typedef struct {
        UsrChatBrowserWidget *chatWgtSet;
        int uid;
        int p2pChannelId;
        netAddr_t dstAddr;
        UnReadImmesgVec ims;
    }FrdWgtBind_t;

    ChatBrowserWgtMan(QWidget *parent);
    ~ChatBrowserWgtMan();

    int openChatBrowserWgtByUid (int uid, int mineUsrId, bool isHasMessage = 0); //返回值 >=0 表示成功新开窗口号，否则表示前次窗口已经打开或者已经达到窗口最高数量
    void closeChatBrowserWgtByUid (int uid);
    void setChatWgtP2PChannelid (int n, int id);
    int recvChatMessage (const ImmessageData &data);

private:
    const int maxWgtNum;
    QWidget *parentWgtPtr;
    FrdWgtBind_t frdChatWgtSet[128];
    P2PUdpChannel *p2pUdpChannelPtr;
    ImmesageChannel *mesgChannelPtr;

    static void p2pChannelNotice(int chid, netAddr_t dstAddr, int type, void *p);
    static int chatWgtOutChatMessage(int windown, const QString &chatMessage, void*p);
    void closeAllChatWgt ();
};

#endif // CHATBROWSERWGTMAN_H
