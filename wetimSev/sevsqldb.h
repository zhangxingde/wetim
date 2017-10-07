#ifndef SEVSQLDB_H
#define SEVSQLDB_H
#include "sql/sqlite3man.h"
#include "singletontemp.h"

class UsrHeadIconDb;

#define FIELD_USRID_ID      "id"
#define FIELD_USRID_NAME    "name"
#define FIELD_USRID_PASSWD  "passwd"
#define FIELD_USRID_AVAICON "avaicon"
#define FIELD_USRID_CRETIME "cretim"
#define TABLE_USRID         "userid"
class SevSqlDB : public SingletonTempBase <SevSqlDB>
{
public:
    friend class SingletonTempBase <SevSqlDB>;
    int getNewUsrID ();
    int getTotalUsrCount ();
    bool getUsrNameByUsrID (int uid, char *dst, unsigned int len);
    bool getUsrPasswdByUsrID (int uid, char *dst, unsigned int len);
    int  getAvaiconIdByUsrID (int uid);
private:
    SevSqlDB();
    ~SevSqlDB();
    Sqlite3Man sqlite3db;
    UsrHeadIconDb *usrHeadIconDbPtr;
    const char* getRandUsrName ();

    inline SqlMan* getSqlDb () {return &sqlite3db;}
};

#endif // SEVSQLDB_H
