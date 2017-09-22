#include <stdio.h>
#include "sevsqldb.h"

SevSqlDB::SevSqlDB():sqlite3db("wetimesev.db")
{
    char sqlcmd[2048];

    if (snprintf(sqlcmd, sizeof(sqlcmd),
                      "create table if not exists '%s'"
                      "("
                        "%s INTEGER PRIMARY KEY NOT NULL,"
                        "%s CHAR(32) not null ,"
                        "%s CHAR(16) not null DEFAULT 123456,"
                        "%s INTEGER not null DEFAULT 1,"
                        "%s INTEGER not null"
                      ")",
                      TABLE_USRID,
                      FIELD_USRID_ID,
                      FIELD_USRID_NAME,
                      FIELD_USRID_PASSWD,
                      FIELD_USRID_AVAICON,
                      FIELD_USRID_CRETIME) > 0){
        sqlite3db.runQuery(sqlcmd);
    }
}

SevSqlDB::~SevSqlDB() {}

int SevSqlDB::getNewUsrID()
{
    int idnew = ((getTotalUsrCount() + 1)<<8) + (time(0)&0xff);
    SqlQueryDataRows row(TABLE_USRID,SqlQueryDataRows::SQLCMD_INSERT);

    row.addOneField(FIELD_USRID_ID, idnew);
    row.addOneField(FIELD_USRID_NAME, "newuser");
    row.addOneField(FIELD_USRID_CRETIME, (long long)time(0));

    if (getSqlDb()->runQuery(row)){
        return idnew;
    }
    return 0;
}

int SevSqlDB::getTotalUsrCount()
{
    SqlQueryDataRows row;

    if (getSqlDb()->runQuery("select count("FIELD_USRID_ID") from "TABLE_USRID, row ) && row.size()){
        return row[0][0].getInt();
    }
    return 0;
}

bool SevSqlDB::getUsrNameByUsrID(int uid, char *dst, unsigned int len)
{
    SqlQueryDataRows row;
    char buf[1024];

    snprintf(buf, sizeof(buf), "select %s from %s where %s=%d",FIELD_USRID_NAME, TABLE_USRID, FIELD_USRID_ID, uid);
    if (dst && getSqlDb()->runQuery(buf, row) && row.size()){
        strncpy(dst, row[0][0].getString(), len);
        return 1;
    }
    return 0;
}

bool SevSqlDB::getUsrPasswdByUsrID(int uid, char *dst, unsigned int len)
{
    SqlQueryDataRows row;
    char buf[1024];

    snprintf(buf, sizeof(buf), "select %s from %s where %s=%d",FIELD_USRID_PASSWD, TABLE_USRID, FIELD_USRID_ID, uid);
    if (dst && getSqlDb()->runQuery(buf, row) && row.size()){
        strncpy(dst, row[0][0].getString(), len);
        return 1;
    }
    return 0;
}

int SevSqlDB::getAvaiconIdByUsrID(int uid)
{
    SqlQueryDataRows row;
    char buf[1024];

    snprintf(buf, sizeof(buf), "select %s from %s where %s=%d",FIELD_USRID_AVAICON, TABLE_USRID, FIELD_USRID_ID, uid);
    if (getSqlDb()->runQuery(buf, row) && row.size()){
        return row[0][0].getInt();
    }
    return -1;
}


















