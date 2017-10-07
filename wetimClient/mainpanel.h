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

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(int uid, QWidget *parent = 0);
    ~MainPanel();
    void startServer ();

    void imMesgRecvKeepAlive(const ImmessageData &m);
    void imMesgPutUsr2UsrListPanel (int uid, const char *name, int avicon);

    int openChatBrowserByUid (int uid, bool isHasMessage = 0);

    void noticeMessagComeing (int uid);
public slots:
    void getUsrOnLinelist ();
    void closeChatBrowserByUid (int uid);

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

    //P2PUdpChannel p2pudpChan;

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


};

#endif // MAINPANEL_H
