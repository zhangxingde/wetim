#ifndef USRCHATBORWERWIDGET_H
#define USRCHATBORWERWIDGET_H

#include <QWidget>
#include <QByteArray>
#include <QString>

class QTextBrowser;
class QTextEdit;
class QHBoxLayout;
class QVBoxLayout;
class QSplitter;
class QLabel;
class QPushButton;


class MainPanel;

class UsrChatBrowserWidget: public QWidget
{
    Q_OBJECT
public:
    UsrChatBrowserWidget(int uid, int mineUid,QWidget *parent);
    void setChatMessageOutFun (int (*f)(int windown, const QString &chatMessage, void*p), void *p)
    {
        chatMessagtOutFun = f;
        chatMessagtOutArg = p;
    }

    void setWindowsNum (int id) {windowNum = id;}
    void pushChatMessageIn (const void *data, int len);
    void setFrdPeerInfo (unsigned int ipv4, unsigned short port);
    int getFrdUid() const {return frdUid;}
    int getMineUid() const {return mineUid;}

private slots:
    void sendOutChatMessageSlot ();
signals:
    void closeChatWgt (int uid);
private:
    QHBoxLayout *layoutFrdInfoPtr;
    QSplitter *chatSplitterPtr;
    QHBoxLayout *layoutBottomBtnPtr;


    QLabel *labFrdNamePtr;
    QLabel *labFrdPeerPtr;

    QTextBrowser *chatTextBrowserPtr;
    QTextEdit *chatTextEditPtr;

    QPushButton *btnSendPtr;
    QPushButton *btnCancelPtr;

    MainPanel *mainPanelPtr;
    int frdUid;
    int mineUid;
    QString frdName;
    QString masterName;
    QByteArray geometryState;
    int windowNum;
    int (*chatMessagtOutFun)(int windown, const QString &chatMessage, void*p);
    void *chatMessagtOutArg;

    void setFrdInfoLayout ();
    void setChatBrowserLayout();
    void setBottomBtnLayout ();
    void setFrdInfoLabels (const QString &name, int avicon);
    void showChatTextInBrowser (const QString &name, const QString &chatData);

    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);

};

#endif // USRCHATBORWERWIDGET_H
