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
#include "usrchatborwerwidget.h"
#include "immessagchannel.h"
#include "friendsetsingleman.h"
#include "mainpanel.h"

UsrChatBrowserWidget::UsrChatBrowserWidget(int uid, QWidget *parent):QWidget(0, Qt::Dialog)
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

    frdUid = uid;

    resize(600,500);

}

void UsrChatBrowserWidget::setFrdInfoLabels(const char *name, int avicon)
{
    labFrdNamePtr->setText(name);
    labFrdNumberPtr->setText(QString::number(frdUid));
}

void UsrChatBrowserWidget::setFrdInfoLayout()
{
    labFrdNamePtr = new QLabel;
    labFrdNumberPtr = new QLabel;

    layoutFrdInfoPtr = new QHBoxLayout;

    layoutFrdInfoPtr->addWidget(labFrdNamePtr);
    layoutFrdInfoPtr->addWidget(labFrdNumberPtr);
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
    btnCancelPtr = new QPushButton(QStringLiteral("关闭"));
    layoutBottomBtnPtr = new QHBoxLayout;

    layoutBottomBtnPtr->addStretch(1);
    layoutBottomBtnPtr->addWidget(btnSendPtr);
    layoutBottomBtnPtr->addWidget(btnCancelPtr);
}

void UsrChatBrowserWidget::closeEvent(QCloseEvent *e)
{
    mainPanelPtr->closeChatBrowserByUid(frdUid);
    e->accept();
}

void UsrChatBrowserWidget::changeEvent(QEvent *e)
{
    if (e->type() != QEvent::WindowStateChange)
        return;
    if (windowState() == Qt::WindowMinimized){
      geometryState =  saveGeometry();
    }
}




























