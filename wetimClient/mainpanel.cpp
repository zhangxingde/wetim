#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QString>
#include <QTimer>
#include <QTextBrowser>
#include <QHostAddress>
#include "mainpanel.h"
#include "usrbaseinfowidget.h"
#include "usrfriendlistwidget.h"
#include "immessagchannel.h"
#include "clientsqldb.h"
#include "climesgobsev.h"
#include "friendsetsingleman.h"
#include "chatbrowserwgtman.h"


#include <QDebug>

MainPanel::MainPanel(int uid, QWidget *parent) :
    QMainWindow(parent),mineUsrId(uid),maxWidth(450)
{
    mesgChannelPtr = ImmesageChannel::getInstance();
    clientSqlDbPtr = ClientSqlDb::getInstance();
    frdsSetManPtr = FriendSetSingleMan::getInstance();
    clockThreadManPtr = ClockThreadMan::getInstance();

    frdsSetManPtr->setMineUid(uid);
    chatBrowserWgtManPtr = new ChatBrowserWgtMan(this);

    setCentralWidget((mainWidgetPtr = new QWidget));
    setUsrBaseInfoWidget();
    setSearchCommBoxWiget();
    setUsrTabWidget();
    setMaximumWidth(getMaxWidth());
    setMinimumHeight(300);


    QVBoxLayout *vlaout = new QVBoxLayout;
    vlaout->addWidget(usrBaseInfoWidgetPtr);
    vlaout->addWidget(searchComBoxPtr);
    vlaout->addWidget(usrTabWidgetPtr);

    mainWidgetPtr->setLayout(vlaout);


    mesgChannelPtr->regOneImobsever(new ImesgKeepAliveObsev, this);
    mesgChannelPtr->regOneImobsever(new ImesgUsrOnlistObsev, this);

    clockThreadManPtr->ClockerInit(&udpKeepTimer,clockTaskUdpKeepAlive, this);
    clockThreadManPtr->addClocker(&udpKeepTimer, 5000);

    curFriendNum = 0;
    remUdpIpv4Addr = remUdpPort = 0;

}

MainPanel::~MainPanel()
{
    delete chatBrowserWgtManPtr;
    clockThreadManPtr->close();
    mesgChannelPtr->close();
    clientSqlDbPtr->close();
    frdsSetManPtr->close();
}

void MainPanel::clockTaskUdpKeepAlive(void *p)
{
   ((MainPanel*)p)->imMesgUdpKeepAliveTimer();
}

void MainPanel::startServer()
{
    imMesgSendWentOnMessage();
    getUsrOnLinelist();

    QHostAddress loc(mesgChannelPtr->getLocalUdpIpv4());
    QString s(QString("loc addr %1:%2").arg(loc.toString()).arg(QString::number(mesgChannelPtr->getLocalUdpProt())));

    debuglogptr->append(s);

}

void MainPanel::imMesgRecvKeepAlive(const ImmessageData &m)
{
    ImmesgDecorUdpKeep udp(&m);

    remUdpIpv4Addr = udp.getIpAddr();
    remUdpPort = udp.getPort();

    debuglogptr->append("keep alive");

}

void MainPanel::imMesgPutUsr2UsrListPanel(int uid, const char *name, int avicon)
{
    if (uid == mineUsrId){
        frdsSetManPtr->instOneFriend2Map(uid, name, avicon);
        return;
    }
    usrFriendListWidgetPtr->addOneFriend(uid, name, avicon);
    ++curFriendNum;

    QString log = "new usr " + QString::number(uid);
    log += " ";
    log += name;
    debuglogptr->append(log);


}

int MainPanel::openChatBrowserByUid(int uid, bool isHasMessage)
{
    int n = chatBrowserWgtManPtr->openChatBrowserWgtByUid(uid, mineUsrId, isHasMessage);
    return n;
}

void MainPanel::closeChatBrowserByUid(int uid)
{
    chatBrowserWgtManPtr->closeChatBrowserWgtByUid(uid);
}

void MainPanel::noticeMessagComeing(int uid)
{
    usrFriendListWidgetPtr->setMessageNote2Uid(uid);
}

void MainPanel::imMesgSendWentOnMessage()
{
    ImmessageData m(IMMESG_USER_BROAD);

    m.setDstSrcUsr(0, mineUsrId);
    mesgChannelPtr->pushUdpDataOut(m);
}

void MainPanel::imMesgUdpKeepAliveTimer()
{
    ImmessageData m(IMMESG_UDP_KEEPALIVE);
    ImmesgDecorUdpKeep udp(&m);

    udp.setDstSrcUsr(0, mineUsrId);
    udp.setUdpAdddr(remUdpIpv4Addr, remUdpPort);
    mesgChannelPtr->pushUdpDataOut(m);
}

void MainPanel::getUsrOnLinelist()
{
    const char *sevaddr;
    int port;

    if ((sevaddr = clientSqlDbPtr->queryLogonSevAddr()) &&
            (port = clientSqlDbPtr->queryLogonSevTcport()) > 0){
        ImmessageData m(IMMESG_USER_ONLIST);
        ImmesgDecorOnlist on(&m);
        on.setDstSrcUsr(0,mineUsrId);
        mesgChannelPtr->pushTcpDataOut(m, sevaddr, port);
    }
}

void MainPanel::setUsrBaseInfoWidget()
{
    usrBaseInfoWidgetPtr = new UsrBaseInfoWidget(this, 80, mineUsrId);


}

void MainPanel::setSearchCommBoxWiget()
{
    searchComBoxPtr = new QComboBox;
    searchComBoxPtr->setEditable(1);
}

void MainPanel::setUsrTabWidget()
{
    usrTabWidgetPtr = new QTabWidget;
    usrTabWidgetPtr->addTab(new QWidget,QStringLiteral("历史"));
    usrTabWidgetPtr->addTab(usrFriendListWidgetPtr = new UsrFriendListWidget(this),QStringLiteral("好友"));
    usrTabWidgetPtr->addTab(new QWidget,QStringLiteral("其他"));
    usrTabWidgetPtr->addTab(debuglogptr = new QTextBrowser, QStringLiteral("日志"));


}

