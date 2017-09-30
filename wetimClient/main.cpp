#include <QApplication>
#include "logonui.h"
#include "mainpanel.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int r = -1;
#if 1
    LogonUi *logon = new LogonUi;
    logon->show();
    r = logon->exec();
    if (r == QDialog::Rejected){
        delete logon;
        return 1;
    }
#endif

    MainPanel *panelPtr = new MainPanel(logon->getMineUsrId());
    delete logon;
    panelPtr->show();
    panelPtr->startServer();
    r = a.exec();
    delete panelPtr;
    return r;
}
