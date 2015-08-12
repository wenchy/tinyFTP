#ifndef _TINYFTP_DATABASE_H_
#define _TINYFTP_DATABASE_H_

#include    "common.h"
#include	<sqlite3.h>

// Database class
class Database
{
public:
	Database(const char * dbfilename);

	Database & create();
	Database & createTable();
	bool execute(const char *sql, Database * pDatabase);
	bool insert(string tblname, map<string, string> & paramMap);
	bool select(string tblname, map<string, string> & paramMap);
	bool update(string tblname, string id, map<string, string> & paramMap);
	bool remove(string tblname, string id);
	vector< map<string ,string> > & getResult();
	//Database & select(string tblname, map<string, string> & paramMap);
	bool first();
	bool find(string tblname, string id);
	bool findALL(string tblname);
	void printResult();
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
	vector< map<string ,string> > resultMapVector;
};

#endif /* _TINYFTP_DATABASE_H_ */
