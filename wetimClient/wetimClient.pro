#-------------------------------------------------
#
# Project created by QtCreator 2017-09-02T20:52:42
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wetimClient
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
QMAKE_CXXFLAGS +=
DEFINES += QT_DEPRECATED_WARNINGS USE_SQLITE3 SQLITE_THREADSAFE SYS_CLIENT
INCLUDEPATH += ..
unix:LIBS += "-ldl"
#win32:INCLUDEPATH += E:\linuxcc\mysql-5.7.19-win32\include
#win32:LIBS += E:\linuxcc\mysql-5.7.19-win32\lib\mysqlclient.lib
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../sql/mysqlman.cpp \
    ../sql/sqlite3man.cpp \
    logonui.cpp \
    main.cpp \
    ../sql/sqlite3/sqlite3.c \
    ../sql/sqldatarows.cpp \
    ../immessage_def.cpp \
    ../gccatomic.cpp \
    tcpconnect.cpp \
    immessagchannel.cpp \
    ../immesgdecor.cpp \
    immessagequeue.cpp \
    clientsqldb.cpp \
    climesgobsev.cpp \
    mainpanel.cpp \
    usrbaseinfowidget.cpp \
    usrfriendlistwidget.cpp \
    udpclient.cpp \
    usrchatborwerwidget.cpp \
    friendsetsingleman.cpp \
    p2pudpchannel.cpp \
    chatbrowserwgtman.cpp \
    clockthreadman.cpp \
    ../usrheadicondb.cpp

HEADERS += \
    ../sql/sqlite3/sqlite3.h \
    ../sql/mysqlman.h \
    ../sql/sqlite3man.h \
    ../sql/sqlman.h \
    logonui.h \
    ../sql/sqldatarows.h \
    ../include_h/sys_defs.h \
    ../immessage_def.h \
    ../gccatomic.h \
    ../include_h/singletontemp.h \
    tcpconnect.h \
    immessagchannel.h \
    ../immesgdecor.h \
    immessagequeue.h \
    clientsqldb.h \
    climesgobsev.h \
    mainpanel.h \
    usrbaseinfowidget.h \
    usrfriendlistwidget.h \
    udpclient.h \
    usrchatborwerwidget.h \
    friendsetsingleman.h \
    p2pudpchannel.h \
    chatbrowserwgtman.h \
    clockthreadman.h \
    ../usrheadicondb.h

SUBDIRS += \
    wetimClient.pro

FORMS += \
    mainpanel.ui
