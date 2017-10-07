#include <cstdio>
#include <cstdlib>
#include "usrheadicondb.h"

char UsrHeadIconDb::sqlpath[256];

bool UsrHeadIconDb::setSQldbPath(const char *path)
{
    if (path && strlen(path) < sizeof(sqlpath)){
        strcpy(sqlpath, path);
        return 1;
    }
    return 0;
}

UsrHeadIconDb::UsrHeadIconDb():headIconDb(sqlpath)
{
    char sqlcmd[2048];

    if (std::snprintf(sqlcmd, sizeof(sqlcmd),
                      "create table if not exists '%s'"
                      "("
                        "%s INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  DEFAULT 0,"
                        "%s INTEGER not null,"
                        "%s BLOB not null"
                      ")",
                      TABLE_NAME_HEADICON,
                      FIELD_HADICON_ID,
                      FIELD_HADICON_TYPE,
                      FIELD_HADICON_BINDATA) > 0){
        headIconDb.runQuery(sqlcmd);
    }
}

UsrHeadIconDb::~UsrHeadIconDb()
{

}

bool UsrHeadIconDb::addOneHeadIcon(int tpe, const void *icon, int len)
{
    SqlQueryDataRows row(TABLE_NAME_HEADICON, SqlQueryDataRows::SQLCMD_INSERT);

    row.addOneField(FIELD_HADICON_TYPE, tpe);
    row.addOneField(FIELD_HADICON_BINDATA, icon, len);
    row.finishedRow();
    return headIconDb.runQuery(row);
}

int UsrHeadIconDb::getHeadIconById(int id, void *dstptr, int &dstlen)
{
    char cmd[256];
    SqlQueryDataRows row;

    snprintf(cmd, sizeof(cmd), "select %s from %s where %s=%d",
             FIELD_HADICON_BINDATA, TABLE_NAME_HEADICON, FIELD_HADICON_ID, id);

    if (headIconDb.runQuery(cmd, row) && row.size()){
        dstlen = row[0][0].getBlobData(dstptr, dstlen);
        return row[0][0].getLength();
    }
    return 0;
}

int UsrHeadIconDb::getRandHeadIconNum()
{
    int n  = getTotalItermNum();

    if (n > 0){
        return (rand() % n) + 1;
    }
    return 1;
}

int UsrHeadIconDb::getTotalItermNum()
{
    char cmd[256];
    SqlQueryDataRows row;

    snprintf(cmd, sizeof(cmd), "select count() from %s", TABLE_NAME_HEADICON);

    if (headIconDb.runQuery(cmd, row) && row.size()){
        return row[0][0].getInt();
    }
    return 0;
}




























