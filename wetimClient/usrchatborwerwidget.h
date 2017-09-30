#ifndef USRCHATBORWERWIDGET_H
#define USRCHATBORWERWIDGET_H

#include <QWidget>
#include <QByteArray>

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
public:
    UsrChatBrowserWidget(int uid, QWidget *parent);
    void setFrdInfoLabels (const char *name, int avicon);

    //void restoreLastGemotery () {restoreGeometry(geometryState);}
private:
    QHBoxLayout *layoutFrdInfoPtr;
    QSplitter *chatSplitterPtr;
    QHBoxLayout *layoutBottomBtnPtr;


    QLabel *labFrdNamePtr;
    QLabel *labFrdNumberPtr;

    QTextBrowser *chatTextBrowserPtr;
    QTextEdit *chatTextEditPtr;

    QPushButton *btnSendPtr;
    QPushButton *btnCancelPtr;

    MainPanel *mainPanelPtr;
    int frdUid;
    QByteArray geometryState;

    void setFrdInfoLayout ();
    void setChatBrowserLayout();
    void setBottomBtnLayout ();

    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);

};

#endif // USRCHATBORWERWIDGET_H
