#include <QApplication>
#include "logonui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogonUi *logon = new LogonUi;
    int r = 0;

    logon->show();

    r = a.exec();
    delete logon;
    return r;
}
