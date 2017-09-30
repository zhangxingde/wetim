#ifndef USRFRIENDLISTWIDGET_H
#define USRFRIENDLISTWIDGET_H
#include <QWidget>
#include <map>

class QStandardItem;
class QModelIndex;

class FriendSetSingleMan;
class UsrChatBrowserWidget;
class MainPanel;

class UsrFriendListWidget : public QWidget
{
    Q_OBJECT
public:
    UsrFriendListWidget(QWidget *parent);
    ~UsrFriendListWidget();
    bool addOneFriend (int uid, const char *name, int avicon);

private slots:
    void showChatTextBorwer (const QModelIndex &index);
private:
    MainPanel *mainPanelPtr;
    QStandardItem *rootNodePtr;
    FriendSetSingleMan *frdsSetManPtr;
    //UsrChatBrowserWidget *chatBrowserWgtPtrSet[128];



};

#endif // USRFRIENDLISTWIDGET_H
