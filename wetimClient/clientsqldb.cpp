#include "clientsqldb.h"
#include "include_h/sys_defs.h"
#include <QDir>
#include <QDebug>

ClientSqlDb::ClientSqlDb():logonDb((QDir::homePath()+"/"+SYSCFGROOT+"/wetim.db").toStdString().c_str())
{
    char sqlcmd[2048];

    if (std::snprintf(sqlcmd, sizeof(sqlcmd),
                      "create table if not exists '%s'"
                      "("
                        "%s INTEGER PRIMARY KEY AUTOINCREMENT  NOT NULL  DEFAULT 1,"
                        "%s CHAR(16) not null ,"
                          "%s INTEGER not null DEFAULT 12315,"
                          "%s INTEGER not null DEFAULT 12316"
                      ")",
                      TABLE_NAME_LOGON,
                      FIELD_LOGON_ID,FIELD_LOGON_SEVADDR, FIELD_LOGON_SEVTCPOR, FIELD_LOGON_SEVUDPOR) > 0){
        logonDb.runQuery(sqlcmd);
    }
    if (std::snprintf(sqlcmd, sizeof(sqlcmd),
                      "create table if not exists '%s'"
                      "("
                        "%s INTEGER PRIMARY KEY NOT NULL,"
                        "%s CHAR(32) not null ,"
                        "%s CHAR(16) not null,"
                        "%s INTEGER not null"
                      ")",
                      TABLE_NAME_BASEINFO,
                      FIELD_USRBASE_ID,FIELD_USRBASE_NMAE, FIELD_USRBASE_PASS, FIELD_USRBASE_AVICON) > 0){
        logonDb.runQuery(sqlcmd);
    }

}
ClientSqlDb::~ClientSqlDb(){}

bool ClientSqlDb::setLogonSevSeting(const char *sev, unsigned short tport, unsigned short uport, const char *oldSevAddr)
{
    SqlQueryDataRows::sqlCmd_t c;
    std::string where;
    if (!oldSevAddr || (oldSevAddr && !oldSevAddr[0])){
        c = SqlQueryDataRows::SQLCMD_INSERT;
        where.clear();
    }else{
        c = SqlQueryDataRows::SQLCMD_UPDATE;
        where.append(FIELD_LOGON_SEVADDR);
        where.append("='");
        where.append(oldSevAddr);
        where.append("'");
    }
   SqlQueryDataRows row(TABLE_NAME_LOGON, c);

   row.addOneField(FIELD_LOGON_SEVADDR, sev);
   row.addOneField(FIELD_LOGON_SEVTCPOR, tport);
   row.addOneField(FIELD_LOGON_SEVUDPOR, uport);
   row.finishedRow();

   return logonDb.runQuery(row,where);
}

bool ClientSqlDb::queryLogonSevAddr(char *dst, unsigned int len)
{
    SqlQueryDataRows rows;
    std::string cmd = "select " +std::string(FIELD_LOGON_SEVADDR) +std::string(" from ") + std::string(TABLE_NAME_LOGON)+std::string(" limit 1");

    if (!dst)
        return 0;
    dst[0] = 0;
    if (logonDb.runQuery(cmd,rows) && rows.size()){
        strncpy(dst, rows[0][0].getString(),len);
        return 1;
    }
    return 0;
}

int ClientSqlDb::queryLogonSevTcport()
{
    SqlQueryDataRows rows;
    std::string cmd = "select " +std::string(FIELD_LOGON_SEVTCPOR) +std::string(" from ") + std::string(TABLE_NAME_LOGON)+std::string(" limit 1");

    if (logonDb.runQuery(cmd,rows) && rows.size()){
        return rows[0][0].getInt();
    }
    return -1;
}

int ClientSqlDb::queryLogonSevUdport()
{
    SqlQueryDataRows rows;
    std::string cmd = "select " +std::string(FIELD_LOGON_SEVUDPOR) +std::string(" from ") + std::string(TABLE_NAME_LOGON) + std::string(" limit 1");

    if (logonDb.runQuery(cmd,rows) && rows.size()){
        return rows[0][0].getInt();
    }
    return -1;
}

bool ClientSqlDb::setUsrBaseInfo(int id, const char *name, const char *pass, int avicon)
{
    SqlQueryDataRows row(TABLE_NAME_BASEINFO, SqlQueryDataRows::SQLCMD_REPLACE);

    row.addOneField(FIELD_USRBASE_ID, id);
    row.addOneField(FIELD_USRBASE_NMAE, name);
    row.addOneField(FIELD_USRBASE_PASS, pass);
    row.addOneField(FIELD_USRBASE_AVICON, avicon);
    row.finishedRow();

    return logonDb.runQuery(row);
}

bool ClientSqlDb::queryAllUsrId(SqlQueryDataRows &row)
{
    std::string cmd = "select " + std::string(FIELD_USRBASE_ID) + std::string(" from ") + std::string(TABLE_NAME_BASEINFO);
    return logonDb.runQuery(cmd, row) && row.size();
}

bool ClientSqlDb::queryUsrPasswdByUsrID(int uid, char *pass, size_t len)
{
    SqlQueryDataRows row;
    char where[128];

    snprintf(where, sizeof(where), FIELD_USRBASE_ID" = %d", uid);

    if (queryUsrBaseInfo(row, where) && row.size() > 0){
        strncpy(pass, row[0][2].getString(), len);
        return 1;
    }
    return 0;
}

bool ClientSqlDb::queryUsrBaseInfo(SqlQueryDataRows &row, const std::string &where)
{
    std::string cmd = "select "+ std::string(FIELD_USRBASE_ID) + std::string(" from ") + std::string(TABLE_NAME_BASEINFO);

    if (!where.empty()){
        cmd += std::string(" where ") + where;
    }
    return logonDb.runQuery(cmd, row);
}
