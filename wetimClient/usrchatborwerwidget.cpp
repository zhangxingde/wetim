#include <QTextBrowser>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSpacerItem>
#include <QPushButton>
#include <QLabel>
#include <QEvent>
#include <QCloseEvent>
#include <QDateTime>
#include <QHostAddress>
#include "usrchatborwerwidget.h"
#include "immessagchannel.h"
#include "friendsetsingleman.h"
#include "mainpanel.h"

UsrChatBrowserWidget::UsrChatBrowserWidget(int uid, int mineUid, QWidget *parent):QWidget(0, Qt::Dialog)
{
    mainPanelPtr = (MainPanel*)parent;

    setFrdInfoLayout();
    setChatBrowserLayout();
    setBottomBtnLayout();

    QVBoxLayout *vbox = new QVBoxLayout(this);

    vbox->addLayout(layoutFrdInfoPtr);
    vbox->addWidget(chatSplitterPtr);
    vbox->addLayout(layoutBottomBtnPtr);

    //setAttribute(Qt::WA_DeleteOnClose);
    FriendSetSingleMan *frdmanPtr = FriendSetSingleMan::getInstance();
    const FriendSetSingleMan::FrdBaseInfo_t *info;
    frdUid = uid;
    this->mineUid = mineUid;
    info = frdmanPtr->findFriendBaseInfo(uid);
    frdName = info ->name;
    setFrdInfoLabels(frdName, info->avicon);
    masterName = frdmanPtr->findFriendBaseInfo(frdmanPtr->getMineUid())->name;
    frdmanPtr->close();

    connect(this, SIGNAL(closeChatWgt(int)), mainPanelPtr, SLOT(closeChatBrowserByUid(int)));

    resize(600,500);

}

void UsrChatBrowserWidget::setFrdInfoLabels(const QString &name, int avicon)
{
    labFrdNamePtr->setText(name);
}

void UsrChatBrowserWidget::setFrdInfoLayout()
{
    labFrdNamePtr = new QLabel;
    labFrdPeerPtr = new QLabel;

    layoutFrdInfoPtr = new QHBoxLayout;

    layoutFrdInfoPtr->addWidget(labFrdNamePtr);
    layoutFrdInfoPtr->addWidget(labFrdPeerPtr);
    layoutFrdInfoPtr->addStretch(1);
}

void UsrChatBrowserWidget::setChatBrowserLayout()
{
    chatSplitterPtr = new QSplitter(Qt::Vertical, 0);
    chatTextBrowserPtr = new QTextBrowser(chatSplitterPtr);
    chatTextEditPtr =  new QTextEdit(chatSplitterPtr);

    chatSplitterPtr->setStretchFactor(0,3);
    chatSplitterPtr->setStretchFactor(1,1);
}

void UsrChatBrowserWidget::setBottomBtnLayout()
{
    btnSendPtr = new QPushButton(QStringLiteral("发送"));
    connect(btnSendPtr, SIGNAL(clicked(bool)), this, SLOT(sendOutChatMessageSlot()));
    btnCancelPtr = new QPushButton(QStringLiteral("关闭"));

    layoutBottomBtnPtr = new QHBoxLayout;

    layoutBottomBtnPtr->addStretch(1);
    layoutBottomBtnPtr->addWidget(btnSendPtr);
    layoutBottomBtnPtr->addWidget(btnCancelPtr);
}

void UsrChatBrowserWidget::closeEvent(QCloseEvent *e)
{
     e->accept();
     emit closeChatWgt(frdUid);

}

void UsrChatBrowserWidget::changeEvent(QEvent *e)
{
    if (e->type() != QEvent::WindowStateChange)
        return;
    if (windowState() == Qt::WindowMinimized){
      geometryState =  saveGeometry();
    }
}

void UsrChatBrowserWidget::pushChatMessageIn(const void *data, int len)
{
    QByteArray srcData((const char*)data,len);

    showChatTextInBrowser(frdName, QString(srcData));
}

void UsrChatBrowserWidget::setFrdPeerInfo(unsigned int ipv4, unsigned short port)
{
    QHostAddress addr(ipv4);

    labFrdPeerPtr->setText("("+addr.toString() + ":" + QString::number(port) +")");
}

void UsrChatBrowserWidget::showChatTextInBrowser(const QString &name, const QString &chatData)
{
    QString text(name);

        text.append("  ");
        text.append('<');
        text.append(QDateTime::currentDateTime().toString());
        text.append('>');
        text.append('\n');
        text.append("  ");
        text.append(chatData);
        chatTextBrowserPtr->append(text);
}

void UsrChatBrowserWidget::sendOutChatMessageSlot()
{
    QString chatContent = chatTextEditPtr->toPlainText();

    if (chatContent.isEmpty())
        return;
    showChatTextInBrowser(masterName,chatContent);
    chatTextEditPtr->clear();

    if (chatMessagtOutFun){
        chatMessagtOutFun(windowNum, chatContent,chatMessagtOutArg);
    }
}




























