#include <QHBoxLayout>
#include <QGridLayout>
#include "usrbaseinfowidget.h"
#include "usrheadicondb.h"
#include "clientsqldb.h"
#include <QDebug>

UsrBaseInfoWidget::UsrBaseInfoWidget(QWidget *parent, int maxH, int uid)
    :QWidget(parent),maxHight(maxH),usrId(uid)
{
    QHBoxLayout *hlayou = new QHBoxLayout;
    UsrHeadIconDb *usrHeadIconDbPtr = UsrHeadIconDb::getInstance();
    ClientSqlDb *clidbptr = ClientSqlDb::getInstance();
    char headData[1024*20], name[46];
    int dstLen = sizeof (headData), len;

    labUsrHeadPtr = new QLabel;
    len = usrHeadIconDbPtr->getHeadIconById(clidbptr->queryUsrHeadIconId(uid), headData, dstLen);
    if (dstLen == len){
        QPixmap pix;

        pix.loadFromData((const uchar *)headData, len);
        labUsrHeadPtr->setPixmap(pix.scaled(QSize(maxHight,maxHight),Qt::KeepAspectRatio));
    }

    hlayou->addWidget(labUsrHeadPtr);
    if (clidbptr->queryUsrName(uid, name, sizeof (name))){
       hlayou->addWidget(new QLabel(name));
    }else{
        hlayou->addWidget(new QLabel(QStringLiteral("无名氏")));
    }
    setLayout(hlayou);

    usrHeadIconDbPtr->close();
    clidbptr->close();
}

