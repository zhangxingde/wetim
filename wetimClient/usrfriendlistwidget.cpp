#include <QTreeView>
#include <QVBoxLayout>
#include <QSpacerItem>

#include "usrfriendlistwidget.h"

UsrFriendListWidget::UsrFriendListWidget(QWidget *parent):
    QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);

    vlayout->addWidget(new QTreeView);
    vlayout->addItem(new QSpacerItem(1,1));
}
