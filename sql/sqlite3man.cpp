#ifdef USE_SQLITE3
#include "sqlite3man.h"
#include "sqlite3/sqlite3.h"

Sqlite3Man::Sqlite3Man(const char *dbFilePath)
{
    sqlite3Ptr = 0;
    strncpy(mDbFilePath, dbFilePath, sizeof(mDbFilePath));
}
Sqlite3Man::~Sqlite3Man()
{
    closeSQlite3();
}

bool Sqlite3Man::open()
{
    if (!sqlite3Ptr &&
            sqlite3_open_v2(mDbFilePath, &sqlite3Ptr, SQLITE_OPEN_CREATE|SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX ,NULL) != SQLITE_OK){
        setErrorString(sqlite3_errmsg(sqlite3Ptr));
        setErrno(sqlite3_errcode(sqlite3Ptr));
    }
    return sqlite3Ptr ? 1:0;
}

void Sqlite3Man::closeSQlite3()
{
    if (sqlite3Ptr){
        sqlite3_close(sqlite3Ptr);
        sqlite3Ptr = 0;
    }
}

bool Sqlite3Man::runQuery(const std::string &cmd)
{
    char *errmsgPtr = 0;

    if (!open() || sqlite3_exec(sqlite3Ptr, cmd.c_str(), 0, 0, &errmsgPtr) != SQLITE_OK){
        setErrno(sqlite3_errcode(sqlite3Ptr));
        if (errmsgPtr){
            setErrorString(errmsgPtr);
            sqlite3_free(errmsgPtr);
        }
        return 0;
    }
    return 1;
}

bool Sqlite3Man::runQuery(const SqlQueryDataRows &queryRows, const std::string &where)
{
    std::string inDataStr;
    unsigned int x = queryRows.x(), y = queryRows.y();
    bool b = 0;

    if (!open() || !x || !y){
        return 0;
    }
    if (queryRows.isSQlCmd(SqlQueryDataRows::SQLCMD_UPDATE)){
        inDataStr = "UPDATE " + std::string(queryRows.getTabName()) + " SET ";
        for (unsigned int i = 0; i < x; ++i){
            inDataStr.append(queryRows.getFiledName(i));
            inDataStr.append("=?,");
        }
        inDataStr.erase(inDataStr.length() -1);
        if (!where.empty()){
            inDataStr += " where " + where;
        }
        b = runStmtQuery(inDataStr, queryRows, 0, y);
    }else if (queryRows.isSQlCmd(SqlQueryDataRows::SQLCMD_INSERT)){
        inDataStr = "insert or ROLLBACK into " + std::string(queryRows.getTabName());
        madeBatchFieldKeyPair(queryRows, inDataStr);
        b = runStmtQuery(inDataStr, queryRows, 0, y);
    }else if (queryRows.isSQlCmd(SqlQueryDataRows::SQLCMD_REPLACE)){
        inDataStr = "insert or replace into " + std::string(queryRows.getTabName());
        madeBatchFieldKeyPair(queryRows, inDataStr);
        b = runStmtQuery(inDataStr, queryRows, 0, y);
    }else{
        setErrno(0);
        setErrorString("unknown SQLCMD");
    }
    return b;
}

void Sqlite3Man::madeBatchFieldKeyPair(const SqlQueryDataRows &queryRows, std::string &tostr)
{
    std::string &inDataStr = tostr;
    unsigned int x = queryRows.x();

    inDataStr.append("(");
    for (unsigned int i = 0; i < x; ++i){
        inDataStr.append(queryRows.getFiledName(i));
        inDataStr.push_back(',');
    }
    inDataStr.erase(inDataStr.length() -1);
    inDataStr.push_back(')');
    inDataStr.append(" values ");

    for (unsigned int m = 0; m < 1; ++m){
        inDataStr.append("(");
        for (unsigned int n = 0; n < x; ++n){
            inDataStr.append("?,");
        }
        inDataStr.erase(inDataStr.length() -1);
        inDataStr.append("),");
    }
    inDataStr.erase(inDataStr.length() -1);
}

bool Sqlite3Man::runStmtQuery(const std::string &cmd, const SqlQueryDataRows &queryRows,
                              const unsigned int startY, const unsigned int nY)
{
    sqlite3_stmt *stmt = NULL;
    const unsigned int x = queryRows.x(), endY = startY + nY;
    int bindRetCode = SQLITE_OK;

    if (sqlite3_prepare_v2(sqlite3Ptr, cmd.c_str(), -1, &stmt, NULL) != SQLITE_OK){
        setErrno(sqlite3_errcode(sqlite3Ptr));
        setErrorString(sqlite3_errmsg(sqlite3Ptr));
        return 0;
    }
    for (unsigned int m = startY; m < endY; ++m){
        for (unsigned int n = 0; n < x && bindRetCode==SQLITE_OK; ++n){
            const SqlQueryDataRows::sqlFiledDesc_t &fieldDes = queryRows.at(n, m);
            switch (fieldDes.type) {
                case SQLDATA_TYPE_INT8:
                    bindRetCode = sqlite3_bind_int(stmt, n+1, fieldDes.getChar());
                    break;
                case SQLDATA_TYPE_STRING:
                    bindRetCode = sqlite3_bind_text(stmt, n+1, fieldDes.getString(), fieldDes.len, 0);
                    break;
                case SQLDATA_TYPE_INT16:
                    bindRetCode = sqlite3_bind_int(stmt, n+1, fieldDes.getInt());
                    break;
                case SQLDATA_TYPE_INT32:
                    bindRetCode = sqlite3_bind_int(stmt, n+1, fieldDes.getInt());
                    break;
                case SQLDATA_TYPE_INT64:
                    bindRetCode = sqlite3_bind_int64(stmt, n+1, fieldDes.getLLint());
                    break;
                case SQLDATA_TYPE_FLOAT:
                    bindRetCode = sqlite3_bind_double(stmt, n+1, fieldDes.getFloat());
                    break;
                case SQLDATA_TYPE_DOUBLE:
                    bindRetCode = sqlite3_bind_double(stmt, n+1, fieldDes.getDouble());
                    break;
                case SQLDATA_TYPE_BLOB:
                    bindRetCode = sqlite3_bind_blob(stmt, n+1, fieldDes.v.s, fieldDes.len, 0);
                    break;
                case SQLDATA_TYPE_NULL:
                    bindRetCode = sqlite3_bind_null(stmt, n+1);
                    break;
                default:
                    setErrorString("unknown SQLDATA_TYPE");
                    bindRetCode = SQLITE_ERROR;
                    setErrno(bindRetCode);
                    sqlite3_finalize(stmt);
                    return 0;
            }
        }
        if ((bindRetCode != SQLITE_OK) || (sqlite3_step(stmt) != SQLITE_DONE)){//插入一排数据为一步(step)
            setErrno(sqlite3_errcode(sqlite3Ptr));
            setErrorString(sqlite3_errmsg(sqlite3Ptr));
            sqlite3_finalize(stmt);
            return 0;
        }
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    return 1;
}

bool Sqlite3Man::runQuery(const std::string &cmd, SqlQueryDataRows &queryRows)
{
    int i, num;
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(sqlite3Ptr,cmd.c_str(), -1,&stmt,NULL) != SQLITE_OK){
        setErrno(sqlite3_errcode(sqlite3Ptr));
        setErrorString(sqlite3_errmsg(sqlite3Ptr));
        return 0;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW){
        num = sqlite3_column_count(stmt);
        for (i = 0; i < num; ++i){
            switch (sqlite3_column_type(stmt, i)){
                case SQLITE_INTEGER :
                    queryRows.addOneField(sqlite3_column_name(stmt, i), (long long int)(sqlite3_column_int64(stmt,i)));
                    break;
                case SQLITE_FLOAT :
                    queryRows.addOneField(sqlite3_column_name(stmt, i), sqlite3_column_double(stmt, i));
                    break;
                case SQLITE_TEXT:
                    queryRows.addOneField(sqlite3_column_name(stmt, i), (const char*)sqlite3_column_text(stmt,i));
                    break;
                case SQLITE_BLOB:
                    queryRows.addOneField(sqlite3_column_name(stmt, i), sqlite3_column_blob(stmt,i),
                                          sqlite3_column_bytes(stmt,i));
                    break;
                case SQLITE_NULL:
                default:
                    queryRows.addOneField(sqlite3_column_name(stmt, i));
                    break;
            }
        }
        queryRows.finishedRow();
    }
    sqlite3_finalize(stmt);
    return true;
}

#endif
