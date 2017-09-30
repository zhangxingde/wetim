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
    QMainWindow(parent),mineUsrId(uid),maxWidth(450),p2pudpChan(this)
{
    qRegisterMetaType<P2PUdpChannel::netAdddr_t>("P2PUdpChannel::netAdddr_t");

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
    mesgChannelPtr->regOneImobsever(new ImesgNetGetUdpAddr, this);
    mesgChannelPtr->regOneImobsever(new ImesgP2pUdp, this);

    clockThreadManPtr->ClockerInit(&udpKeepTimer,5000, clockTaskUdpKeepAlive, this);
    clockThreadManPtr->addClocker(&udpKeepTimer);

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
    usrFriendListWidgetPtr->addOneFriend(uid, name, avicon);
    ++curFriendNum;

    QString log = "new usr " + QString::number(uid);
    log += " ";
    log += name;
    debuglogptr->append(log);


}

void MainPanel::imMesgGetUdpAddr(const ImmessageData &m)
{
    ImmesgDecorNetwkUdpAddr udp(&m);

    if (!udp.isAck()){//来自对端的 IMMESG_NETGET_UDPADDR 请求P2P请求本地端口，将要进行回复
        sendLocalUdpAddr2Usr(udp.getSrcUsrId(), 0);
    }//然后开发p2p隧道打通

    QHostAddress addr(udp.getSrcUsrRemUdpAddr().ipv4), addr0(udp.getSrcUsrLocUdpAddr().ipv4);
    QString log("Udp addr ");

    log+= addr0.toString();

    log+=":";
    log+=QString::number(udp.getSrcUsrLocUdpAddr().port);
    log+="->";
    log+=addr.toString();
    log+=":";
    log+=QString::number(( udp.getSrcUsrRemUdpAddr().port));
    debuglogptr->append(log);

    P2PUdpChannel::netAdddr_t rem, loc;

    rem.ipv4 = udp.getSrcUsrRemUdpAddr().ipv4;
    rem.port = udp.getSrcUsrRemUdpAddr().port;
    loc.ipv4 = udp.getSrcUsrLocUdpAddr().ipv4;
    loc.port = udp.getSrcUsrLocUdpAddr().port;

    p2pudpChan.getChannelId(rem, loc, udp.getSrcUsrId(), mineUsrId);


}

void MainPanel::imMesgP2pUdp(const ImmessageData &m)
{
    p2pudpChan.recvData(m);
}

int MainPanel::openChatBrowserByUid(int uid)
{
    int n = chatBrowserWgtManPtr->openChatBrowserWgtByUid(uid);

    if (n >= 0){
        sendLocalUdpAddr2Usr(uid);
    }
    return n;
}

void MainPanel::closeChatBrowserByUid(int uid)
{
    chatBrowserWgtManPtr->closeChatBrowserWgtByUid(uid);
}

void MainPanel::imMesgSendWentOnMessage()
{
    ImmessageData m(IMMESG_USER_BROAD);

    m.setDstSrcUsr(0, mineUsrId);
    mesgChannelPtr->pushUdpDataOut(m);
}

void MainPanel::sendLocalUdpAddr2Usr(int dstUid, int isInit)
{
    ImmessageData m(IMMESG_NETGET_UDPADDR);
    ImmesgDecorNetwkUdpAddr udp(&m);

    udp.setDstSrcUsr(dstUid, mineUsrId);
    udp.setSrcUsrLocUdpAddr(mesgChannelPtr->getLocalUdpIpv4(), mesgChannelPtr->getLocalUdpProt());
    udp.setAck(!isInit);
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

