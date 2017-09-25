#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QString>
#include <QTimer>
#include <QTextBrowser>
#include "mainpanel.h"
#include "usrbaseinfowidget.h"
#include "usrfriendlistwidget.h"
#include "immessagchannel.h"
#include "clientsqldb.h"
#include "climesgobsev.h"

#include <QDebug>

MainPanel::MainPanel(int uid, QWidget *parent) :
    QMainWindow(parent),mineUsrId(uid),maxWidth(450)
{
    mesgChannelPtr = ImmesageChannel::getInstance();
    clientSqlDbPtr = ClientSqlDb::getInstance();

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

    imMesgSendWentOnMessage();
    curFriendNum = 0;
    getUsrOnLinelist();

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(imMesgUdpKeepAliveTimerSlot()));
    t->start(5000);
}

MainPanel::~MainPanel()
{
    mesgChannelPtr->close();
    clientSqlDbPtr->close();
}

void MainPanel::imMesgRecvKeepAlive(const ImmessageData &m)
{
    debuglogptr->append("keep alive");

}

void MainPanel::imMesgPutUsr2UsrListPanel(int uid, const char *name, int avicon)
{
    ++curFriendNum;

    QString log = "new usr " + QString::number(uid);
    log += " ";
    log += name;
    debuglogptr->append(log);


}

void MainPanel::imMesgSendWentOnMessage()
{
    const char *sevaddr;
    int uport;

    if ((sevaddr = clientSqlDbPtr->queryLogonSevAddr()) &&
            (uport = clientSqlDbPtr->queryLogonSevUdport())> 0){
        ImmessageData m(IMMESG_USER_BROAD);

        m.setDstSrcUsr(0, mineUsrId);
        mesgChannelPtr->pushUdpDataOut(m, sevaddr, uport);
    }
}

void MainPanel::imMesgUdpKeepAliveTimerSlot()
{
    const char *sevaddr;
    int uport;

    if ((sevaddr = clientSqlDbPtr->queryLogonSevAddr()) &&
            (uport = clientSqlDbPtr->queryLogonSevUdport())> 0){
        ImmessageData m(IMMESG_UDP_KEEPALIVE);

        m.setDstSrcUsr(0, mineUsrId);
        mesgChannelPtr->pushUdpDataOut(m, sevaddr, uport);
    }
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

