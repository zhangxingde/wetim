#ifndef USRFRIENDLISTWIDGET_H
#define USRFRIENDLISTWIDGET_H
#include <QWidget>
#include <QStandardItem>
#include <map>
#include <QPixmap>

#include "clockthreadman.h"
class QStandardItem;
class QModelIndex;

class FriendSetSingleMan;
class UsrChatBrowserWidget;
class MainPanel;

class UsrFriendListWidget : public QWidget
{
    Q_OBJECT
public:
    typedef struct {
        int uid;
        QPixmap pix;
        QStandardItem *itermPtr;
        int hasMessage;
    }FrdNode_t;
    typedef std::map<int, FrdNode_t> FrdNodeMap;

    UsrFriendListWidget(QWidget *parent);
    ~UsrFriendListWidget();
    bool addOneFriend (int uid, const char *name, int avicon);
    bool setMessageNote2Uid (int uid);

private slots:
    void showChatTextBorwer (const QModelIndex &index);
    void setMessageNoticeSlot();
signals:
    void setMessageNoticeSig (int uid);
private:
    MainPanel *mainPanelPtr;
    QStandardItem *rootNodePtr;
    FriendSetSingleMan *frdsSetManPtr;
    class QTreeView *tree;
    FrdNodeMap frdNodes;

    ClockThreadMan::clocker_list messgeNoticeTimer;
    ClockThreadMan *clockManPtr;

    static void messageNoticeFun (void *p);


    bool insertFrdNode (int uid, QStandardItem *i, QPixmap &pix);
    FrdNode_t* getFrdNode (int uid)
    {
        FrdNodeMap::iterator it = frdNodes.find(uid);
        if (it != frdNodes.end()){
            return &it->second;
        }
        return 0;
    }
    void delFrdNode (int uid)
    {
        FrdNodeMap::iterator it = frdNodes.find(uid);
        if (it != frdNodes.end()){
            frdNodes.erase(it);
        }
    }


};

#endif // USRFRIENDLISTWIDGET_H
