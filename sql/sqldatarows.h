#ifndef _SQLDATAFORMAT_H
#define _SQLDATAFORMAT_H
#include <string.h>
#include <stdint.h>
class SqlQueryDataRows {
	public:
		enum {
			SQLDATA_TYPE_INT8 = 0,
			SQLDATA_TYPE_INT16,
			SQLDATA_TYPE_INT32,
			SQLDATA_TYPE_INT64,
			SQLDATA_TYPE_FLOAT,
			SQLDATA_TYPE_DOUBLE,
			SQLDATA_TYPE_STRING,
			SQLDATA_TYPE_ZERO,
			SQLDATA_TYPE_BLOB,
			SQLDATA_TYPE_NULL,
			__SQLDATA_TYPE_MAX_
		};
#define SQLDATA_TYPE_INT8 SqlQueryDataRows::SQLDATA_TYPE_INT8
#define SQLDATA_TYPE_INT16 SqlQueryDataRows::SQLDATA_TYPE_INT16
#define SQLDATA_TYPE_INT32 SqlQueryDataRows::SQLDATA_TYPE_INT32
#define SQLDATA_TYPE_INT64 SqlQueryDataRows::SQLDATA_TYPE_INT64
#define SQLDATA_TYPE_FLOAT SqlQueryDataRows::SQLDATA_TYPE_FLOAT
#define SQLDATA_TYPE_DOUBLE SqlQueryDataRows::SQLDATA_TYPE_DOUBLE
#define SQLDATA_TYPE_STRING SqlQueryDataRows::SQLDATA_TYPE_STRING
#define SQLDATA_TYPE_ZERO SqlQueryDataRows::SQLDATA_TYPE_ZERO
#define SQLDATA_TYPE_BLOB SqlQueryDataRows::SQLDATA_TYPE_BLOB
#define SQLDATA_TYPE_NULL SqlQueryDataRows::SQLDATA_TYPE_NULL

		typedef union {
			char ch;
			int i32;
			unsigned int u32;
			long int ilong;
			unsigned long int uilong;
			long long int i64;
			unsigned long long int u64;
			float f;
			double d;
			void *p;
			char s[1];
		}sqlDataValue_t;

		typedef struct _sqlFiledDesc{
			int len;
			unsigned char type;
                        int orgStrOffset;
			const char *name;
			sqlDataValue_t v;
			char ss[1];
			void setValue (char ch, const char *orgstr){
				v.ch = ch;
				type = SQLDATA_TYPE_INT8;
				len = sizeof (ch);
				setOrgString(len, orgstr);
			}
			void setValue (int i, const char *orgstr){
				v.i32 = i;
				type = SQLDATA_TYPE_INT32;
				len = sizeof (i);
				setOrgString(len, orgstr);
			}
			void setValue (long long int i, const  char *orgstr){
				v.i64 = i;
				type = SQLDATA_TYPE_INT64;
				len = sizeof(i);
				setOrgString(len, orgstr);
			}
			void setValue (float f, const char *orgstr){
				v.f = f;
				type = SQLDATA_TYPE_FLOAT;
				len = sizeof (f);
				setOrgString(len, orgstr);
			}
			void setValue (double d, const char *orgstr){
				v.d = d;
				type = SQLDATA_TYPE_DOUBLE;
				len = sizeof (d);
				setOrgString(len, orgstr);
			}
			void setValue (const void *p, unsigned int l){
				memcpy(v.s, p, l);
				type = SQLDATA_TYPE_BLOB;
				len = l;
                                setOrgString(len, 0);
			}
			void setValue (const char *s, const char *orgstr){
				strcpy(v.s,s);
				type = SQLDATA_TYPE_STRING;
				len = strlen (s);
                                setOrgString(len, orgstr);
			}
			void setValue() {
				type = SQLDATA_TYPE_NULL;
				len = 0;
				v.u64 = 0;
                                setOrgString(len, 0);
			}
			void setOrgString (int offset, const char *s){
                                orgStrOffset = offset + 1;
                                if (s){
                                    strcpy(v.s + orgStrOffset, s);
                                }
			}
            int getLength () const {return len;}
			char getChar () const{return v.ch;}
			int  getInt () const {return v.i32;} 
			long long int getLLint () const {return v.i64;} 
			float getFloat () const {return v.f;} 
			double getDouble () const {return v.d;}
			const char* getString () const {return v.s;}
            int  getBlobData (void *p, int maxlen) const{
                int cplen = len > maxlen ? maxlen : len;
                memcpy(p, v.s, cplen);
                return cplen;
			} 
                        const char* getOrgString () const {return v.s + orgStrOffset;}

		}sqlFiledDesc_t;

		typedef enum {
			SQLCMD_NONE = 0,
			SQLCMD_INSERT, 
            SQLCMD_UPDATE,
            SQLCMD_REPLACE
		}sqlCmd_t;

        SqlQueryDataRows ();
        SqlQueryDataRows (const char *tbname, sqlCmd_t c);
        ~SqlQueryDataRows();

		bool addOneField (const char *name, char ch, const char *orgstr = 0);
		bool addOneField (const char *name, int i, const char *orgstr = 0);
		bool addOneField (const char *name, long long li, const char *orgstr = 0);
		bool addOneField (const char *name, const char *s, const char *orgstr = 0);
		bool addOneField (const char *name, float f, const char *orgstr = 0);
		bool addOneField (const char *name, double d, const char *orgstr = 0);
        bool addOneField(const char *name, const void *p, unsigned long long len);
        bool addOneField(const char *name);
		
		bool finishedRow () {
			addColNameFinished = 1;
			if (curRowSize + 1 >= maxRowSize){
				maxRowSize = 2*maxRowSize;
				Row *buf = new Row[maxRowSize];
				if (!buf)
					return 0;
				memcpy(buf, rowmemPtr, sizeof (Row)*curRowSize);
				if (needFreeRowMemBuf)
					delete[] rowmemPtr;
				else
					needFreeRowMemBuf = 1;
				rowmemPtr = buf;
			}
			rowmemPtr[curRowSize].fieldStartIndex = curFieldNum - rowmemPtr[curRowSize].m_size;
			rowmemPtr[curRowSize].colNameStartIndex = curColNum - rowmemPtr[curRowSize].m_size;
			if (curRowSize ? rowmemPtr[curRowSize].m_size != rowmemPtr[curRowSize-1].m_size : 0){
				return 0;  
			}
			curRowSize++;
			rowmemPtr[curRowSize].m_size = 0;
			return 1;
		}//再一次增加相同的字段组
        unsigned int y() const { return  curColNum > 0 ? curFieldNum / curColNum : 0;}
        unsigned int x() const { return curColNum;}
        const char* getFiledName (int x) const {return memptr + colNamePosBuf[x];}
		
        const sqlFiledDesc_t& at(int x, int y) const {
			sqlFiledDesc_t *p = (sqlFiledDesc_t*)(memptr + fieldMemPosPtr[y*this->x() + x]);
			return *p;
		}
		void setCanRecvOrgStr (bool b) { canRecvOrgStr = b;}

        const char *getTabName ()  const {return tabnamefor;}
        bool isSQlCmd (sqlCmd_t c) const { return !(curSQlCmd ^ c);}
		void reset();
        typedef struct _Row{
            friend class SqlQueryDataRows;
			public:
				uint32_t size() { return m_size;}
				const char* name (int i) {
					return  memptr + (colNamePosBuf)[colNameStartIndex + i];
				}
				const sqlFiledDesc_t& operator[](int i){
					sqlFiledDesc_t *p = (sqlFiledDesc_t*)(memptr + (fieldMemPosPtr)[fieldStartIndex + i]);
					p->name = memptr + (colNamePosBuf)[colNameStartIndex + i];
					return *p;
				}
			private:
				uint32_t m_size;
				int fieldStartIndex;
				int colNameStartIndex;
				char *memptr;
				int *fieldMemPosPtr;
				int *colNamePosBuf; 
		}Row;
        uint32_t size() const {return curRowSize;}
		uint32_t getFieldNum() {return curFieldNum;}
		Row& operator[](int i){
			Row &r = rowmemPtr[i];
			
			r.memptr = memptr;
			r.fieldMemPosPtr = fieldMemPosPtr;
			r.colNamePosBuf = colNamePosBuf;
			return r;
		}
	private:
		char membuf[64*1024]; //主要是当频繁小量查询时，不需要进行内存申请，提高效率；
		int  fieldMemPosBuf[1024];//每个field相对于memptr的偏移量
		int  colNamePosBuf[512];//每张表的最多字段列表512
		Row  rows[1024];
		char *memptr;
		int  *fieldMemPosPtr;
		Row *rowmemPtr;
		int curDataLen; //当前数据长度
		int curFieldNum;//当前字段数
		int  maxMemLen;//最大的内存长度，curDataLen 不得超过此值
		int  maxFieldNum;//最大的字段值, curFieldNum 不得超过此值
		int needFreeMembuf, needFreeFiledMemPodbuf, needFreeRowMemBuf;
		int curColNum, maxColNum;
		int addColNameFinished;
        char tabnamefor[48];
		bool canRecvOrgStr;
		sqlCmd_t curSQlCmd;
		uint32_t curRowSize, maxRowSize;
	
        void setDefault ();

		inline char* getNewSpaceFromMembufByLen (int len)
		{
			int chgLen = len;
			char *p = 0;
			if (curDataLen + chgLen >= maxMemLen){
				maxMemLen = 2*(curDataLen + chgLen);
				char *buf = new char [maxMemLen];
				if (!buf)
					return 0;
				memcpy(buf, memptr, curDataLen);
				if (needFreeMembuf){
					delete[] memptr;
				}else{
					needFreeMembuf = 1;
				}
				memptr = buf;
			}
			p = memptr + curDataLen;
			curDataLen += chgLen;
			memset(p, 0, chgLen);
			return p;
		}

		inline sqlFiledDesc_t* getNewSpaceForFiledByLen (int len)
		{
			char *s = getNewSpaceFromMembufByLen (len + sizeof (sqlFiledDesc_t));
			sqlFiledDesc_t *p = 0;
			int offsert = 0;

			if (!s)
				return 0;
			offsert = s - memptr;
			if (curFieldNum + 1 >= maxFieldNum){
				maxFieldNum = 2*maxFieldNum;
				int *a = new int [maxFieldNum];
				if (!a)
					return 0;
				memcpy(a, fieldMemPosPtr, curFieldNum * sizeof (int));
				if (needFreeFiledMemPodbuf){
					delete[] fieldMemPosPtr;
				}else{
					needFreeFiledMemPodbuf = 1;
				}
				fieldMemPosPtr = a;
			}
			fieldMemPosPtr[curFieldNum++] = offsert;
			rowmemPtr[curRowSize].m_size++;
			p = (sqlFiledDesc_t*)(s);
			return p;
		}

		inline void addColNameIn (const char *name)
		{
			char *s = 0;

			if (addColNameFinished)
				return;
			s = getNewSpaceFromMembufByLen(strlen(name) + 1);
			if (s){
				colNamePosBuf[curColNum++] = s - memptr;
				strcpy(s, name);
			}
		}

};
#endif


























