#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include "include_h/sys_defs.h"
#include "logonui.h"
#include "climesgobsev.h"

LogonUi::LogonUi():QDialog(0, Qt::Window),
    hight(300), width(430),cfgPath(QDir::homePath()+"/"+SYSCFGROOT)
{
    dbptr = ClientSqlDb::getInstance();
    setDisplayLogLayout();
    setLogonInfoLayout();
    setSysSetingLayout1();
    setSysSetingLayout2();

    topHalfPtr = new QVBoxLayout;
    topHalfPtr->addWidget(displayLabelPtr);

    downHalfPtr = new QVBoxLayout;
    downHalfPtr->addLayout(logonInfoHlayoutPtr);
    downHalfPtr->addWidget(sysSetingBoxPtr1);


    mainLayoutPtr = new QVBoxLayout();
    mainLayoutPtr->addLayout(topHalfPtr);
    mainLayoutPtr->addLayout(downHalfPtr);
    setLayout(mainLayoutPtr);

    setWindowTitle(QStringLiteral("用户登录"));
    setFixedHeight(hight);
    setFixedWidth(width);

    QDir cfgDir(cfgPath);

    if (!cfgDir.exists()){
        cfgDir.mkpath(cfgPath);
    }

    imChannelPtr = ImmesageChannel::getInstance();
    imChannelPtr->regOneImobsever(new ImesgApplyNumObsev, this);
    imChannelPtr->regOneImobsever(new ImesgLononObsev, this);
    connect(imChannelPtr, SIGNAL(sockerror(int, const QString&)), this, SLOT(dispNetworkErrorInfo(int, const QString&)));
    connect(this, SIGNAL(logonAuthStateSig(int)), this, SLOT(logonAuthStateSlot(int)));
    oldSevAddr[0] = 0;
}

LogonUi::~LogonUi()
{
    dbptr->close();
    imChannelPtr->close();
}

void LogonUi::dispNetworkErrorInfo(int errn, const QString &errstr)
{
    QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("%1, 错误码 %2").arg(errstr).arg(errn), QMessageBox::Ok);
    logOnBtnPtr->setDisabled(0);
}

void LogonUi::setApplyNumInfo(const char *name, const char *pass, int uid, int avicon)
{
    QString userinfo = QStringLiteral("用户名：") + name;

    userinfo += QStringLiteral("\n密  码：") + pass;
    userinfo += QStringLiteral("\n账  号：") + QString::number(uid);
    displayLabelPtr->clear();
    displayLabelPtr->setText(userinfo);
    dbptr->setUsrBaseInfo(uid, name, pass, avicon);

    reflashUsrDropDownlist();
}

void LogonUi::logonAuthStateSlot(int succ)
{
    logOnBtnPtr->setDisabled(0);
    if (!succ){
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("登录认证失败"),QMessageBox::Ok);
    }else{
        accept();
    }
}

void LogonUi::setLogonAuthState(int succd)
{
    emit logonAuthStateSig(succd);
}

void LogonUi::setDisplayLogLayout()
{
    displayLabelPtr = new QLabel(QStringLiteral("广告信息"));
    displayLabelPtr->setFixedSize(QSize(width, hight/2));
    QFont f;
    f.setBold(0);
    f.setPointSize(15);
    displayLabelPtr->setFont(f);
}

void LogonUi::setLogonInfoLayout()
{
    logonInfoHlayoutPtr = new QHBoxLayout;
    QHBoxLayout *hlayoutPtr = logonInfoHlayoutPtr;

    QLabel *l = new QLabel();
    l->setFixedSize(QSize(width/5, hight/2));
    hlayoutPtr->addWidget(l);

    QGridLayout *layoutGridLogonPtr = new QGridLayout;
    layoutGridLogonPtr->addWidget(labUserIdPtr= new QLabel(QStringLiteral("账 号")),0, 0);
    lineUserName.setEditable(1);
    reflashUsrDropDownlist();

    layoutGridLogonPtr->addWidget(&lineUserName,0,1);

    applyNumBtnPtr = new QPushButton(QStringLiteral("申请号码"));
    connect(applyNumBtnPtr, SIGNAL(clicked(bool)), this, SLOT(applyNumFromServer()));
    layoutGridLogonPtr->addWidget(applyNumBtnPtr,0,2);
    layoutGridLogonPtr->addWidget(labUserPasswdPtr = new QLabel(QStringLiteral("密  码")),1, 0);
    layoutGridLogonPtr->addWidget(&linePasswd,1, 1);
    logOnBtnPtr = new QPushButton(QStringLiteral("登 录"));
    logOnBtnPtr->setDefault(1);
    connect(logOnBtnPtr, SIGNAL(clicked(bool)), this, SLOT(sendLogonMessage()));
    layoutGridLogonPtr->addWidget(logOnBtnPtr, 2,1);
    hlayoutPtr->addLayout(layoutGridLogonPtr);
}

void LogonUi::setSysSetingLayout1()
{
    sysSetingBoxPtr1 = new QGroupBox;
    QHBoxLayout *sysSetingHlayoutPtr1 = new QHBoxLayout;
    setingBtnPtr = new QPushButton (QStringLiteral("服务器设置"));
    setingBtnPtr->setFixedWidth(70);
    connect(setingBtnPtr, SIGNAL(clicked(bool)), this, SLOT(dispSeting()));

    sysSetingHlayoutPtr1->addWidget(setingBtnPtr,Qt::AlignLeft);
    sysSetingHlayoutPtr1->addStretch();
    sysSetingHlayoutPtr1->setMargin(0);
    sysSetingBoxPtr1->setLayout(sysSetingHlayoutPtr1);
}

void LogonUi::setSysSetingLayout2()
{
    sysSetingBoxPtr2 = new QGroupBox;
    QHBoxLayout *sysSetingHlayoutPtr2 = new QHBoxLayout;

    setServerAddrLinePtr = new QLineEdit();
    setServerAddrLinePtr->setInputMask("000.000.000.000");
    setServerAddrLinePtr->setFixedWidth(105);

    setServerTportLinePtr = new QLineEdit();
    setServerTportLinePtr->setFixedWidth(35);
    setServerTportLinePtr->setInputMask("00000");

    setServerUportLinePtr = new QLineEdit;
    setServerUportLinePtr->setFixedWidth(35);
    setServerUportLinePtr->setInputMask("00000");

    setServerAddrLabelPtr = new QLabel(QStringLiteral("服务器"));
    setServerTPortLablPtr = new QLabel (QStringLiteral("TCP端口"));
    setServerUPortLablPtr = new QLabel(QStringLiteral("UDP端口"));
    sysSetingHlayoutPtr2->addWidget(setServerAddrLabelPtr);
    sysSetingHlayoutPtr2->addWidget(setServerAddrLinePtr);
    //sysSetingHlayoutPtr2->addSpacing(220);

    sysSetingHlayoutPtr2->addWidget(setServerTPortLablPtr);
    sysSetingHlayoutPtr2->addWidget(setServerTportLinePtr);

    sysSetingHlayoutPtr2->addWidget(setServerUPortLablPtr);
    sysSetingHlayoutPtr2->addWidget(setServerUportLinePtr);

    setServerOkBtnPtr = new QPushButton(QStringLiteral("确认"));
    setServerOkBtnPtr->setFixedWidth(35);
    setServerCancleBtnPtr = new QPushButton(QStringLiteral("取消"));
    setServerCancleBtnPtr->setFixedWidth(35);
    connect(setServerOkBtnPtr, SIGNAL(clicked(bool)), this, SLOT(getLogonSevSeting()));
    connect(setServerCancleBtnPtr, SIGNAL(clicked(bool)), this, SLOT(dispSeting()));
    sysSetingHlayoutPtr2->addWidget(setServerOkBtnPtr);
    sysSetingHlayoutPtr2->addWidget(setServerCancleBtnPtr);


    sysSetingHlayoutPtr2->setMargin(0);

    sysSetingBoxPtr2->setLayout(sysSetingHlayoutPtr2);
    sysSetingBoxPtr2->setFlat(0);


}

void LogonUi::dispSeting()
{
    static int clkSetingBtn = 1;

    QGroupBox *sysSetingBoxPtrs[2] = {sysSetingBoxPtr2, sysSetingBoxPtr1};
    lineUserName.setDisabled(clkSetingBtn);
    linePasswd.setDisabled(clkSetingBtn);
    applyNumBtnPtr->setDisabled(clkSetingBtn);
    logOnBtnPtr->setDisabled(clkSetingBtn);
    sysSetingBoxPtrs[clkSetingBtn]->hide();
    downHalfPtr->replaceWidget(sysSetingBoxPtrs[clkSetingBtn], sysSetingBoxPtrs[!clkSetingBtn]);
    sysSetingBoxPtrs[!clkSetingBtn]->show();
    if (clkSetingBtn){
        int tport, uport;
        const char *addr;

        if ((addr = dbptr->queryLogonSevAddr()) &&
            (tport = dbptr->queryLogonSevTcport()) > 0 &&
                (uport = dbptr->queryLogonSevUdport()) > 0){
            setDisplayLogonSevInfo(addr, tport, uport);
        }
    }
    clkSetingBtn = !clkSetingBtn;
}

void LogonUi::getLogonSevSeting()
{
    bool b;
    dispSeting();

    b = dbptr->setLogonSevSeting(setServerAddrLinePtr->text().toStdString().c_str(),
                                 setServerTportLinePtr->text().toUInt(),
                                 setServerUportLinePtr->text().toUInt(), oldSevAddr);
    if (!b){
        dispNetworkErrorInfo(0, QStringLiteral("设置登陆服务器信息失败:") + QString(dbptr->getErrorStr()));
    }
}

void LogonUi::setDisplayLogonSevInfo(const char *sev, int tport, int uport)
{
    if (sev){
        strncpy(oldSevAddr, sev, sizeof(oldSevAddr));
        setServerAddrLinePtr->setText(sev);
        setServerTportLinePtr->setText(QString::number(tport));
        setServerUportLinePtr->setText(QString::number(uport));
    }
}

void LogonUi::applyNumFromServer()
{
    int tport, uport;
    const char *addr;

    if ((addr = dbptr->queryLogonSevAddr()) &&
        (tport = dbptr->queryLogonSevTcport()) > 0 &&
            (uport = dbptr->queryLogonSevUdport()) > 0){
        ImmessageData m(IMMESG_APPLYNUM);
        //imChannelPtr->sendTcpData2Server(oldSevAddr, tport, m);
        imChannelPtr->pushTcpDataOut(m, addr, tport);
    }else{
        dispNetworkErrorInfo(0, QStringLiteral("查询登陆服务器信息失败") + QString(dbptr->getErrorStr()));
    }

}

void LogonUi::reflashUsrDropDownlist()
{
    SqlQueryDataRows row;

    if (dbptr->queryAllUsrId(row)){
        lineUserName.clear();
        for (uint32_t i = 0; i < row.size(); ++i){
            lineUserName.addItem(QString::number(row[i][0].getInt()));
        }
    }
}

void LogonUi::sendLogonMessage()
{
    ImmessageData m(IMMESG_USER_LOGON);
    ImmesgDecorLogon logon(&m);
    int tport;
    const  char *addr;


    if ((addr = dbptr->queryLogonSevAddr()) &&
            (tport = dbptr->queryLogonSevTcport()) > 0){
        logon.setDstSrcUsr(0, lineUserName.currentText().toInt());
        logon.setLogonPassword(linePasswd.text().toStdString().c_str());
        imChannelPtr->pushTcpDataOut(m, addr,tport);
        logOnBtnPtr->setDisabled(1);
    }else{
        dispNetworkErrorInfo(0, QStringLiteral("查询登陆服务器信息失败"));
    }

}

















