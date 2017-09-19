#ifdef USE_MYSQL
#include "mysqlman.h"
#include <iostream>
#include <cstdio>
#include <stdlib.h>

MysqlMan::MysqlMan()
{
    mysqlPtr = 0;
    mErrno = 0;
    stmt = 0;
}

MysqlMan::~MysqlMan ()
{
    mysql_stmt_close(stmt);
    mysql_close(mysqlPtr);
}

bool MysqlMan::setConnect (const char *usr, const char *passwd, const char *addr, int port , const char *dbname)
{
    const int ok = 1, timeout = 5;

    mysqlPtr = mysql_init (NULL);
    mysql_options(mysqlPtr, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    mysql_options(mysqlPtr, MYSQL_OPT_RECONNECT, &ok);
    mysql_options(mysqlPtr, MYSQL_SET_CHARSET_NAME, "utf8");
    //mysql_set_character_set(mysqlPtr, "utf8");
    if(!mysql_real_connect(mysqlPtr,addr, usr, passwd, dbname, port, 0, 
                CLIENT_MULTI_STATEMENTS|CLIENT_IGNORE_SIGPIPE) ||
            !(stmt = mysql_stmt_init (mysqlPtr))){
       setErrorString(mysql_error(mysqlPtr));
       setErrno(mysql_errno(mysqlPtr));
       mysql_close(mysqlPtr);
       mysqlPtr = 0;
       return 0;
    }
    mysql_query(mysqlPtr,"set character_set_results='utf8'");
	mysql_query(mysqlPtr, "set character_set_client='utf8'");
	mysql_query(mysqlPtr, "set character_set_database='utf8'");
    mysql_query(mysqlPtr, "SET character_set_connection='utf8'");
    return 1;
}

bool MysqlMan::runQuery(const std::string &cmd)
{
    if (!mysqlPtr){
        //setErrorString("未连接数据库");
        return 0;
    }
    if(mysql_real_query(mysqlPtr, cmd.c_str (), cmd.length ())){
		setErrorString(mysql_error(mysqlPtr));
        setErrno(mysql_errno(mysqlPtr));
        return 0;
    }
    return 1;
}
bool MysqlMan::runQuery(const SqlQueryDataRows &rows, const std::string &where)
{
    std::string inDataStr("insert into ");
    unsigned int x = rows.x(), y = rows.y();
    const unsigned int  maxEachRows = sizeof(maxBind)/sizeof (maxBind[0])/x;
    bool isUpdate = rows.isSQlCmd(SqlQueryDataRows::SQLCMD_UPDATE), b = 0;

    if (!mysqlPtr){
        //setErrorString("未连接数据库");
        return 0;
    }
    if (!x || !y){
        setErrno(0);
        setErrorString((std::string("在该表项中未输入任何数据:")+std::string(rows.getTabName())).c_str());
        return 0;
    }
    inDataStr += rows.getTabName();
    inDataStr.append("(");
    for (unsigned int i = 0; i < x; ++i){
        inDataStr.append(rows.getFiledName(i));
        inDataStr.push_back(',');
    }
    inDataStr.erase(inDataStr.length() -1);
    inDataStr.push_back(')');
    inDataStr.append(" values ");

    unsigned int valLength = inDataStr.length(), r, n = 0;

    do {
        inDataStr.erase(valLength -1);
        r = y > maxEachRows ? maxEachRows:y;
        for (unsigned int m = 0; m < r; ++m){
            inDataStr.append("(");
            for (unsigned int n = 0; n < x; ++n){
                inDataStr.append("?,");
            }
            inDataStr.erase(inDataStr.length() -1);
            inDataStr.append("),");
        }
        inDataStr.erase(inDataStr.length() -1);
        if (isUpdate){
            inDataStr.append(" ON DUPLICATE KEY UPDATE ");
            for (unsigned int i = 0; i < x; ++i){
                  inDataStr.append(rows.getFiledName(i));
                  inDataStr.append("=values(");
                  inDataStr.append(rows.getFiledName(i));
                  inDataStr.append("),");
            }
            inDataStr.erase(inDataStr.length() -1);
        }
        y -= r;
    }while ((b = runStmtQuery(inDataStr, rows,n++*maxEachRows, r)) && y/maxEachRows);
    return b;
}

bool MysqlMan::runStmtQuery(const std::string &cmd, const SqlQueryDataRows &formatData,
                            const unsigned int startY, const unsigned int nY)
{
    const unsigned x = formatData.x(), endY = startY + nY;
    MYSQL_BIND *bind = maxBind;

    if (!stmt || mysql_stmt_prepare(stmt, cmd.c_str(), cmd.length())
              || mysql_stmt_param_count(stmt) != nY*x){
        setErrorString(mysql_stmt_error(stmt));
        setErrno(mysql_stmt_errno(stmt));
        return 0;
    }
    for (unsigned int m = startY; m < endY; ++m){
        for (unsigned int n = 0; n < x; ++n){
            const SqlQueryDataRows::sqlFiledDesc_t &fieldDes = formatData.at(n, m);
            const unsigned int xy = n + m*x;
            switch (fieldDes.type) {
                case SQLDATA_TYPE_INT8:
                case SQLDATA_TYPE_STRING:
                    bind[xy].buffer_type = MYSQL_TYPE_STRING;
                    bind[xy].buffer_length = fieldDes.len;
                    break;
                case SQLDATA_TYPE_INT16:
                    bind[xy].buffer_type = MYSQL_TYPE_SHORT;
                    break;
                case SQLDATA_TYPE_INT32:
                    bind[xy].buffer_type = MYSQL_TYPE_LONG;
                    break;
                case SQLDATA_TYPE_INT64:
                    bind[xy].buffer_type = MYSQL_TYPE_LONGLONG;
                    break;
                case SQLDATA_TYPE_FLOAT:
                    bind[xy].buffer_type = MYSQL_TYPE_FLOAT;
                    break;
                case SQLDATA_TYPE_DOUBLE:
                    bind[xy].buffer_type = MYSQL_TYPE_DOUBLE;
                    break;
                case SQLDATA_TYPE_BLOB:
                    bind[xy].buffer_type = MYSQL_TYPE_BLOB;;
                    bind[xy].buffer_length = fieldDes.len;
                    break;
                case SQLDATA_TYPE_NULL:
                    bind[xy].buffer_type = MYSQL_TYPE_NULL;
                    bind[xy].buffer_length = fieldDes.len;
                    break;
                default:
                    setErrorString("unknown SQLDATA_TYPE");
                    return 0;
            }
            bind[xy].buffer = (void*)fieldDes.v.s;
            bind[xy].buffer_length = fieldDes.len;
            bind[xy].is_null = 0;
        }
    }
    bool ret = 1;
    if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)){
            setErrorString(mysql_stmt_error(stmt));
            setErrno(mysql_stmt_errno(stmt));
            ret = 0;
    }
    return ret;
}

bool MysqlMan::runQuery(const std::string &cmd, SqlQueryDataRows &formatData)
{
    MYSQL_RES *res = NULL;
    MYSQL_ROW row;

    if (!mysqlPtr){
        //setErrorString("未连接数据库");
        return 0;
    }
    if(mysql_real_query(mysqlPtr, cmd.c_str (), cmd.length ())){
		setErrorString(mysql_error(mysqlPtr));
        setErrno(mysql_errno(mysqlPtr));
        return 0;
    }
    if(!(res = mysql_use_result(mysqlPtr))){
		setErrorString(mysql_error(mysqlPtr));
        setErrno(mysql_errno(mysqlPtr));
        return 0;
    }

    MYSQL_FIELD *fields;
    unsigned long *lens;
    unsigned int n ;

    while ((row = mysql_fetch_row(res))){
        lens = mysql_fetch_lengths (res);
        fields = mysql_fetch_fields (res);
        n = mysql_num_fields(res);
        for (unsigned int i = 0; i < n; ++i){
			if (!row[i]){
				formatData.addOneField(fields[i].name);
				continue;
			}
            switch (fields[i].type){
                case MYSQL_TYPE_LONG:
                case MYSQL_TYPE_SHORT:
                    formatData.addOneField(fields[i].name, atoi(row[i]), row[i]);
                    break;
                case MYSQL_TYPE_VAR_STRING:
                case MYSQL_TYPE_STRING:
					formatData.addOneField(fields[i].name, row[i], row[i]);
                    break;
                case MYSQL_TYPE_FLOAT:
                case MYSQL_TYPE_DOUBLE:
                    formatData.addOneField(fields[i].name, atof(row[i]), row[i]);
                    break;
				case MYSQL_TYPE_LONGLONG:
                    formatData.addOneField(fields[i].name, atoll(row[i]), row[i]);
                    break;
                case MYSQL_TYPE_BLOB:
                    formatData.addOneField(fields[i].name, row[i], lens[i]);
                    break;
				default:
					formatData.addOneField(fields[i].name, row[i], row[i]);
					break;
			}
		}
		formatData.finishedRow();
	}
    mysql_free_result(res);
    return 1;
}
#endif
