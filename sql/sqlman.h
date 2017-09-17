#ifndef SQLMAN_H
#define SQLMAN_H
#include <string>
#include "sqldatarows.h"

class SqlMan
{
public:
    SqlMan() {
        errStr.clear();
        mErrno = 0;
    }
    virtual ~SqlMan() {}
    virtual bool runQuery (const std::string &cmd) = 0;
    virtual bool runQuery(const SqlQueryDataRows &rows, const std::string &where = std::string()) = 0;
    virtual bool runQuery(const std::string &cmd, SqlQueryDataRows &rows) = 0;

    const char* getError () {return errStr.c_str();}
    unsigned int getErrno () {return mErrno;}
protected:
    std::string errStr;
    unsigned int mErrno;
    void setErrorString (const char *e) {errStr = e; }
    void setErrno (unsigned int eno) {mErrno = eno;}
};
#endif // SQLMAN_H
