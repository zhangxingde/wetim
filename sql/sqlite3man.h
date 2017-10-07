#ifndef SQLITE3MAN_H
#define SQLITE3MAN_H
#ifdef USE_SQLITE3
#include "sqlman.h"

class Sqlite3Man : public SqlMan
{
public:
    Sqlite3Man(const char *dbFilePath);
    ~Sqlite3Man();
    bool open();
    bool runQuery (const std::string &cmd);
    bool runQuery (const SqlQueryDataRows &queryRows, const std::string &where = std::string());
    bool runQuery(const std::string &cmd, SqlQueryDataRows &queryRows);
private:
    struct sqlite3 *sqlite3Ptr;
    char mDbFilePath[128];

    void closeSQlite3();
    void madeBatchFieldKeyPair (const SqlQueryDataRows &queryRows, std::string &tostr);
    bool runStmtQuery(const std::string &cmd, const SqlQueryDataRows &formatData,
                      const unsigned int startY, const unsigned int nY);
};
#endif
#endif // SQLITE3MAN_H
