#include <QTreeView>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QBrush>
#include <QPixmap>

#include <QDebug>

#include "usrfriendlistwidget.h"
#include "usrchatborwerwidget.h"
#include "friendsetsingleman.h"
#include "mainpanel.h"
#include "usrheadicondb.h"


UsrFriendListWidget::UsrFriendListWidget(QWidget *parent):
    QWidget(parent)
{
    mainPanelPtr = (MainPanel*)(parent);
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    tree = new QTreeView;
    QStandardItemModel *standardModelPtr = new QStandardItemModel;

    standardModelPtr->setColumnCount(2);
    standardModelPtr->setHeaderData(0,Qt::Horizontal, QStringLiteral("我的伙伴"));
    standardModelPtr->setHeaderData(1,Qt::Horizontal, "");

    rootNodePtr = standardModelPtr->invisibleRootItem();

    tree->setModel(standardModelPtr);
    connect(tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showChatTextBorwer(const QModelIndex &)));

    vlayout->addWidget(tree);

    frdsSetManPtr = FriendSetSingleMan::getInstance();
    clockManPtr = ClockThreadMan::getInstance();
    clockManPtr->ClockerInit(&messgeNoticeTimer, messageNoticeFun, this);

    connect(this, SIGNAL(setMessageNoticeSig(int)),this, SLOT(setMessageNoticeSlot()));
}

UsrFriendListWidget::~UsrFriendListWidget()
{
    if (frdsSetManPtr)
        frdsSetManPtr->close();
}

bool UsrFriendListWidget::addOneFriend(int uid, const char *name, int avicon)
{
    if (frdsSetManPtr->instOneFriend2Map(uid, name, avicon)){
        QPixmap pix;
        unsigned char headData[20*1024];
        int dstlen = sizeof (headData), rlen;
        UsrHeadIconDb *db = UsrHeadIconDb::getInstance();

        rlen = db->getHeadIconById(avicon, headData, dstlen);
        if (rlen == dstlen){
            pix.loadFromData(headData, rlen);
        }
        QList<QStandardItem *> rows;
        QStandardItem *icon = new QStandardItem(pix,"");
        QStandardItem *f = new QStandardItem(name);

        icon->setEditable(0);
        icon->setData(uid);
        f->setEditable(0);
        f->setData(uid);

        rows<<icon<<f;
        rootNodePtr->appendRow(rows);

        insertFrdNode(uid, icon, pix);
        return 1;
    }
    return 0;
}

bool UsrFriendListWidget::setMessageNote2Uid(int uid)
{
    FrdNode_t *f = getFrdNode(uid);

    if (!f)
        return 0;
    f->hasMessage = 1;
    return clockManPtr->addClocker(&messgeNoticeTimer, 200);
}

void UsrFriendListWidget::setMessageNoticeSlot()
{
    static unsigned int x = 0;
    FrdNodeMap::iterator it;
    FrdNodeMap &m = frdNodes;
    bool isConntione = 0;


    ++x;
    for (it = m.begin(); it != m.end(); ++it){
        if (it->second.hasMessage){
            isConntione = 1;
            QStandardItem *i = it->second.itermPtr;
            if (x%2){
                   i->setIcon(QIcon());
            }else{
                i->setIcon(it->second.pix);
            }
        }
    }
    if (!isConntione){
        clockManPtr->delCloker(&messgeNoticeTimer);
    }
}

void UsrFriendListWidget::messageNoticeFun (void *p)
{
    UsrFriendListWidget *wgt = (UsrFriendListWidget*)p;
    emit wgt->setMessageNoticeSig(0);
}

void UsrFriendListWidget::showChatTextBorwer(const QModelIndex &index)
{
    const QStandardItem *f = rootNodePtr->child(index.row());
    int uid;
    bool isHasMessage = 0;
    FrdNode_t *node;
    if (!f)
        return;
    uid = f->data().toInt();
    node = getFrdNode(uid);
    if (node && node ->hasMessage){
        node ->hasMessage = 0;
        isHasMessage = 1;
    }
    mainPanelPtr->openChatBrowserByUid(f->data().toInt(), isHasMessage);
}

bool UsrFriendListWidget::insertFrdNode(int uid, QStandardItem *i, QPixmap &pix)
{
    FrdNode_t n;
    std::pair<FrdNodeMap::iterator, bool> ret;

    memset(&n, 0, sizeof(n));
    n.itermPtr = i;
    n.uid =uid;
    n.pix = pix;

    ret = frdNodes.insert(std::pair<int, FrdNode_t>(uid, n));
    return ret.second;
}


























