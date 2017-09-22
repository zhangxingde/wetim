#include <QHBoxLayout>
#include <QGridLayout>
#include "usrbaseinfowidget.h"

UsrBaseInfoWidget::UsrBaseInfoWidget(QWidget *parent, int maxH, int uid)
    :QWidget(parent),maxHight(maxH),usrId(uid)
{
    QHBoxLayout *hlayou = new QHBoxLayout;

    labUsrHeadPtr = new QLabel;
    QPixmap pix("D:/rar/g/headicon/1.jpg");

    labUsrHeadPtr->setPixmap(pix.scaled(QSize(maxHight,maxHight),Qt::KeepAspectRatio));

    hlayou->addWidget(labUsrHeadPtr);
    hlayou->addWidget(new QLabel(QStringLiteral("张三")));

    setLayout(hlayou);
}

