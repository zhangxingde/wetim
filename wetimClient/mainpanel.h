#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QMainWindow>

#include "clockthreadman.h"

class QComboBox;
class QTabWidget;
class QTextBrowser;

class UsrBaseInfoWidget;
class UsrFriendListWidget;
class ImmesageChannel;
class ClientSqlDb;
class ImmessageData;
class FriendSetSingleMan;
class ChatBrowserWgtMan;
class ClockThreadMan;

#include "p2pudpchannel.h"

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(int uid, QWidget *parent = 0);
    ~MainPanel();
    void startServer ();

    void imMesgRecvKeepAlive(const ImmessageData &m);
    void imMesgPutUsr2UsrListPanel (int uid, const char *name, int avicon);
    void imMesgGetUdpAddr (const ImmessageData &m);
    void imMesgP2pUdp (const ImmessageData &m);

    int openChatBrowserByUid (int uid);
    void closeChatBrowserByUid (int uid);
public slots:
    void getUsrOnLinelist ();

private slots:

private:
    int const mineUsrId;
    int const maxWidth;
    QWidget *mainWidgetPtr;
    UsrBaseInfoWidget *usrBaseInfoWidgetPtr;
    QComboBox *searchComBoxPtr;
    QTabWidget *usrTabWidgetPtr;
    UsrFriendListWidget *usrFriendListWidgetPtr;
    QTextBrowser *debuglogptr;

    ImmesageChannel *mesgChannelPtr;
    ClientSqlDb *clientSqlDbPtr;
    FriendSetSingleMan *frdsSetManPtr;
    ChatBrowserWgtMan *chatBrowserWgtManPtr;

    ClockThreadMan *clockThreadManPtr;
    ClockThreadMan::clocker_list udpKeepTimer;

    P2PUdpChannel p2pudpChan;

    int curFriendNum;

    unsigned int remUdpIpv4Addr;
    unsigned int remUdpPort;

    static void clockTaskUdpKeepAlive (void *p);

    void imMesgUdpKeepAliveTimer ();

    int getMaxWidth () {return maxWidth;}
    void setUsrBaseInfoWidget();
    void setSearchCommBoxWiget ();
    void setUsrTabWidget();

    void imMesgSendWentOnMessage ();
    void sendLocalUdpAddr2Usr (int dstUid, int isInit = 1);


};

#endif // MAINPANEL_H
