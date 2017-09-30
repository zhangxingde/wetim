#include <QTreeView>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QItemSelectionModel>

#include <QDebug>

#include "usrfriendlistwidget.h"
#include "usrchatborwerwidget.h"
#include "friendsetsingleman.h"
#include "mainpanel.h"


UsrFriendListWidget::UsrFriendListWidget(QWidget *parent):
    QWidget(parent)
{
    mainPanelPtr = (MainPanel*)(parent);
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QTreeView *tree = new QTreeView;
    QStandardItemModel *standardModelPtr = new QStandardItemModel;

    standardModelPtr->setColumnCount(1);
    standardModelPtr->setHeaderData(0,Qt::Horizontal, QStringLiteral("我的伙伴"));

    rootNodePtr = standardModelPtr->invisibleRootItem();

    tree->setModel(standardModelPtr);
    connect(tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showChatTextBorwer(const QModelIndex &)));

    vlayout->addWidget(tree);

    frdsSetManPtr = FriendSetSingleMan::getInstance();
}

UsrFriendListWidget::~UsrFriendListWidget()
{
    if (frdsSetManPtr)
        frdsSetManPtr->close();
}

bool UsrFriendListWidget::addOneFriend(int uid, const char *name, int avicon)
{
    if (frdsSetManPtr->instOneFriend2Map(uid, name, avicon)){
        QStandardItem *f = new QStandardItem(QString("%1 %2").arg(QString::number(uid)).arg(name));

        f->setEditable(0);
        f->setData(uid);
        rootNodePtr->appendRow(f);
        return 1;
    }
    return 0;
}

void UsrFriendListWidget::showChatTextBorwer(const QModelIndex &index)
{
    const QStandardItem *f = rootNodePtr->child(index.row());
    if (!f)
        return;
    mainPanelPtr->openChatBrowserByUid(f->data().toInt());
}


























