#ifndef USRBASEINFOWIDGET_H
#define USRBASEINFOWIDGET_H
#include <QWidget>
#include <QLabel>


class UsrBaseInfoWidget : public QWidget
{
public:
    UsrBaseInfoWidget(QWidget *parent, int maxH, int uid);

private:
    const int maxHight;
    const int usrId;
    QLabel *labUsrHeadPtr;
};

#endif // USRBASEINFOWIDGET_H
