#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include <QMainWindow>

class UsrBaseInfoWidget;
class QComboBox;
class QTabWidget;
class UsrFriendListWidget;
class ImmesageChannel;
class ClientSqlDb;
class ImmessageData;
class QTextBrowser;

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(int uid, QWidget *parent = 0);
    ~MainPanel();

    void imMesgRecvKeepAlive(const ImmessageData &m);
    void imMesgPutUsr2UsrListPanel (int uid, const char *name, int avicon);
public slots:
    void getUsrOnLinelist ();

private slots:
    void imMesgUdpKeepAliveTimerSlot ();

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

    int curFriendNum;


    int getMaxWidth () {return maxWidth;}
    void setUsrBaseInfoWidget();
    void setSearchCommBoxWiget ();
    void setUsrTabWidget();

    void imMesgSendWentOnMessage ();


};

#endif // MAINPANEL_H
