#include "sqldatarows.h"

SqlQueryDataRows::SqlQueryDataRows()
{
    setDefault();
	curSQlCmd = SQLCMD_NONE;
}

SqlQueryDataRows::SqlQueryDataRows(const char *tbname, sqlCmd_t c)
{
    setDefault();

	curSQlCmd = c;
    memset(tabnamefor, 0, sizeof(tbname));
    strncpy(tabnamefor, tbname, sizeof (tabnamefor));
}

SqlQueryDataRows::~SqlQueryDataRows()
{
	if (needFreeMembuf)
		delete[] memptr;
	if (needFreeFiledMemPodbuf)
		delete[] fieldMemPosPtr;
	if (needFreeRowMemBuf)
		delete[] rowmemPtr;	
}

void SqlQueryDataRows::setDefault()
{
    memptr = membuf;
    maxMemLen = sizeof(membuf);
    curDataLen = 0;

    fieldMemPosPtr = fieldMemPosBuf;
    maxFieldNum = sizeof (fieldMemPosBuf) / sizeof (fieldMemPosBuf[0]);
    curFieldNum = 0;

    needFreeMembuf = needFreeFiledMemPodbuf = 0;

    curColNum = 0;
    maxColNum = sizeof (colNamePosBuf) / sizeof (colNamePosBuf[0]);
    addColNameFinished = 0;

    tabnamefor[0] = 0;
	canRecvOrgStr = 0;

	rowmemPtr = rows;
	curRowSize = 0;
	maxRowSize = sizeof(rows)/sizeof(rows[0]);
	needFreeRowMemBuf = 0;
	rows[0].m_size = 0;
}

void SqlQueryDataRows::reset()
{
	if (needFreeMembuf)
		delete[] memptr;  
	if (needFreeFiledMemPodbuf)
		delete [] fieldMemPosPtr;
	if (needFreeRowMemBuf)
		delete[] rowmemPtr;
	setDefault();
}

bool SqlQueryDataRows::addOneField(const char *name, char ch, const char *orgstr)
{
	bool canorgstr = canRecvOrgStr && orgstr; 
	sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(sizeof (ch) + (canorgstr ? strlen(orgstr) : 0));

	if (!sqlDescPtr)
		return 0;
	sqlDescPtr->setValue(ch, canorgstr?orgstr: NULL);
	addColNameIn(name);
	return 1;
}
bool SqlQueryDataRows::addOneField (const char *name, int i, const char *orgstr)
{
	bool canorgstr = canRecvOrgStr && orgstr; 
	sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(sizeof (i) + (canorgstr ? strlen(orgstr) : 0));

	if (!sqlDescPtr)
		return 0;
	sqlDescPtr->setValue(i, canorgstr?orgstr:NULL);
	addColNameIn(name);
	return 1;
}
bool SqlQueryDataRows::addOneField (const char *name, long long int li, const char *orgstr)
{
	bool canorgstr = canRecvOrgStr && orgstr; 
	sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(sizeof (li) + (canorgstr?strlen(orgstr): 0));

	if (!sqlDescPtr)
		return 0;
	sqlDescPtr->setValue(li, canorgstr?orgstr:NULL);
	addColNameIn(name);
	return 1;
}
bool SqlQueryDataRows::addOneField (const char *name, const char *s, const char *orgstr)
{
	bool canorgstr = canRecvOrgStr && orgstr; 
	sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(strlen(s) + (canorgstr?strlen(orgstr):0));

	if (!sqlDescPtr)
		return 0;
	sqlDescPtr->setValue(s, canorgstr?orgstr:NULL);
	addColNameIn(name);
	return 1;
}
bool SqlQueryDataRows::addOneField (const char *name, float f, const char *orgstr)
{
	bool canorgstr = canRecvOrgStr && orgstr; 
	sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(sizeof (f) + (canorgstr ? strlen(orgstr):0));

	if (!sqlDescPtr)
		return 0;
	sqlDescPtr->setValue(f, canorgstr?orgstr:NULL);
	addColNameIn(name);
	return 1;
}
bool SqlQueryDataRows::addOneField (const char *name, double d, const char *orgstr)
{
	bool canorgstr = canRecvOrgStr && orgstr; 
	sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(sizeof (d) + (canorgstr?strlen(orgstr):0));

	if (!sqlDescPtr)
		return 0;
	sqlDescPtr->setValue(d, canorgstr?orgstr:NULL);
	addColNameIn(name);
	return 1;
}

bool SqlQueryDataRows::addOneField(const char *name, const void *p, unsigned long long len)
{
    sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(len);

    if (!sqlDescPtr)
        return 0;
    sqlDescPtr->setValue(p, len);
    addColNameIn(name);
    return 1;
}
bool SqlQueryDataRows::addOneField(const char *name)
{
    sqlFiledDesc_t *sqlDescPtr = getNewSpaceForFiledByLen(0);

    if (!sqlDescPtr)
        return 0;
    sqlDescPtr->setValue();
    addColNameIn(name);
    return 1;
}
