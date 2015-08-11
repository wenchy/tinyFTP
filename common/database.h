#ifndef _TINYFTP_DATABASE_H_
#define _TINYFTP_DATABASE_H_

#include    "common.h"
#include	"../include/sqlite3.h"

using namespace std;

#define		DBFILENAME	"tinyFTP.db"

// Database class
class Database
{
public:
	Database(const char * dbfilename);

	Database & createTable();
	bool execute(const char *sql, Database * pDatabase);
	bool insert(string tblname, map<string, string> & paramMap);
	bool select(string tblname, map<string, string> & paramMap);
	bool update(string tblname, string id, map<string, string> & paramMap);
	bool remove();
	map<string ,string> & getResult();
	bool first();
	bool find(string tblname, string id);
	bool findALL(string tblname);
	void printResult(string sqlAction);
	void clean();
	void dump();

	void init();

	~Database();

private:
	std::string dbFilename;
	string sqlSring;
	sqlite3 *pDb;
	char *zErrMsg;
	int  rc;
	map<string ,string> resultMap;
};

#endif /* _TINYFTP_DATABASE_H_ */
