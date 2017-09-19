#ifndef ClientSqlDb_H
#define ClientSqlDb_H
#include "sql/sqlite3man.h"
#include "include_h/singletontemp.h"

#define TABLE_NAME_LOGON      "logonsev"
#define FIELD_LOGON_ID        "id"
#define FIELD_LOGON_SEVADDR   "sevaddr"
#define FIELD_LOGON_SEVTCPOR  "tport"
#define FIELD_LOGON_SEVUDPOR  "uport"

#define TABLE_NAME_BASEINFO   "usrbaseinfo"
#define FIELD_USRBASE_ID      "id"
#define FIELD_USRBASE_NMAE    "name"
#define FIELD_USRBASE_PASS    "passwd"
#define FIELD_USRBASE_AVICON  "avicon"


class ClientSqlDb : public SingletonTempBase<ClientSqlDb>
{
public:
    friend class SingletonTempBase<ClientSqlDb>;

    int getErrorNo () {return logonDb.getErrno();}
    const char* getErrorStr () {return logonDb.getError();}
    bool setLogonSevSeting (const char *sev, unsigned short tport, unsigned short uport, const char *oldSevAddr);
    bool queryLogonSevAddr (char *dst, unsigned int len);
    int  queryLogonSevTcport ();
    int  queryLogonSevUdport ();

    bool setUsrBaseInfo (int id, const char *name, const char *pass, int avicon);
    bool queryAllUsrId (SqlQueryDataRows &row);
    bool queryUsrPasswdByUsrID (int uid, char *pass, size_t len);
private:
    ClientSqlDb();
    ~ClientSqlDb();
    Sqlite3Man logonDb;

    bool queryUsrBaseInfo (SqlQueryDataRows &row, const std::string &where = std::string());
};

#endif // ClientSqlDb_H
