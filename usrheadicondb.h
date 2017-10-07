#ifndef USRHEADICONDB_H
#define USRHEADICONDB_H
#include "sql/sqlite3man.h"
#include "include_h/singletontemp.h"

#define TABLE_NAME_HEADICON      "usrheadicon"
#define FIELD_HADICON_ID         "id"
#define FIELD_HADICON_TYPE       "type"
#define FIELD_HADICON_BINDATA    "bindata"

class UsrHeadIconDb:public SingletonTempBase<UsrHeadIconDb>
{
    friend class SingletonTempBase<UsrHeadIconDb>;
public:
    enum {
        HADICO_TYPE_MAN = 1,
        HADICO_TYPE_WOMAN,
        HADICO_TYPE_ANIMAL
    };
    static bool setSQldbPath (const char *path);
    bool addOneHeadIcon (int tpe, const void *icon, int len);
    int getHeadIconById(int id, void *dstptr, int &dstlen); //返回头像数据的长度
    int getRandHeadIconNum ();
private:
    UsrHeadIconDb();
    ~UsrHeadIconDb();
    static char sqlpath[256];
    Sqlite3Man headIconDb;

    int getTotalItermNum ();
};

#endif // USRHEADICONDB_H
