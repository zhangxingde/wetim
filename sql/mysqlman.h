#ifndef MYSQLMAN_H
#define MYSQLMAN_H

#ifdef USE_MYSQL
#include "sqlman.h"
#include <mysql.h>
#include <string>


class MysqlMan : public SqlMan
{
public:
    MysqlMan();
    ~MysqlMan();
	bool setConnect (const char *usr, const char *passwd, const char *addr, int port , const char *dbname);
    bool runQuery (const std::string &cmd);
    bool runQuery(const SqlQueryDataRows &rows, const std::string &where = std::string());
    bool runQuery(const std::string &cmd, SqlQueryDataRows &formatData);

private:
    MYSQL *mysqlPtr;
    MYSQL_STMT    *stmt;
    MYSQL_BIND maxBind[4096];

    bool runStmtQuery(const std::string &cmd, const SqlQueryDataRows &formatData,
                      const unsigned int startY, const unsigned int nY);
};
#endif
#endif // MYSQLMAN_H

