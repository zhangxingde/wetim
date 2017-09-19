#include <QApplication>
#include "logonui.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LogonUi *logon = new LogonUi;
    int r = 0;

    logon->show();
    r = logon->exec();
    delete logon;
    if (r == QDialog::Rejected){
        return 1;
    }
    r = a.exec();
    return r;
}
