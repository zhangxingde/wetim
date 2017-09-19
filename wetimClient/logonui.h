#ifndef LOGONUI_H
#define LOGONUI_H
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

#include "clientsqldb.h"
#include "immessagchannel.h"

class LogonUi : public QDialog
{
    Q_OBJECT
public:
    LogonUi();
    ~LogonUi();
    void setApplyNumInfo (const char *name, const char *pass, int uid, int avicon);
    void setLogonAuthState (int succd);
private slots:
    void dispSeting ();
    void getLogonSevSeting ();
    void applyNumFromServer ();
    void dispNetworkErrorInfo (int errn, const QString &errstr);
    void sendLogonMessage ();
    void logonAuthStateSlot (int succ);
signals:
    void logonAuthStateSig (int succ);
private:
    const int hight, width;
    QVBoxLayout *topHalfPtr,*downHalfPtr;
    QLabel *displayLabelPtr;
    QHBoxLayout *logonInfoHlayoutPtr;
    QGroupBox *sysSetingBoxPtr1, *sysSetingBoxPtr2;
    class QPushButton *setingBtnPtr;
    QVBoxLayout *mainLayoutPtr;
    QComboBox lineUserName;
    QLabel *labUserIdPtr;
    QLineEdit linePasswd;
    QLabel *labUserPasswdPtr;
    class QPushButton *applyNumBtnPtr;
    class QPushButton *logOnBtnPtr;

    QLineEdit *setServerAddrLinePtr;
    QLineEdit *setServerTportLinePtr;
    QLineEdit *setServerUportLinePtr;
    QPushButton *setServerOkBtnPtr;
    QPushButton *setServerCancleBtnPtr;
    QLabel *setServerAddrLabelPtr;
    QLabel *setServerTPortLablPtr;
    QLabel *setServerUPortLablPtr;

    ClientSqlDb *dbptr;
    const QString cfgPath;
    ImmesageChannel *imChannelPtr;

    void setDisplayLogLayout ();
    void setLogonInfoLayout();
    void setSysSetingLayout1 ();
    void setSysSetingLayout2 ();

    void setDisplayLogonSevInfo (const char *sev, int tport, int uport);
    void reflashUsrDropDownlist ();
    char oldSevAddr[20];
};

#endif // LOGONUI_H
