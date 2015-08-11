#include "database.h"

// static int pre_callback(void *data, int argc, char **argv, char **azColName){
//    fprintf(stdout, "%s: \n", (const char*)data);
//    for(int i=0; i<argc; i++){
//       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//    printf("\n");
//    return 0;
// }

static int callback(void *pDatabase, int argc, char **argv, char **azColName){
   map<string ,string> & kvMap = ((Database *)pDatabase)->getResult();
   for(int i=0; i<argc; i++){
      kvMap.insert( std::pair<string, string>(azColName[i], argv[i]) );
   }
   printf("\n");
   return 0;
}

Database::Database(const char * zDbFilename): dbFilename(zDbFilename)
{
   zErrMsg = NULL;
   /* Open database */
   rc = sqlite3_open(dbFilename.c_str(), &pDb);
   if( rc ){
     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(pDb));
     exit(0);
   }else{
     fprintf(stdout, "Open database successfully\n");
   }
}

void Database::init()
{
   std::map<string, string> selectParamMap = {  {"id", "1"}, {"username", "Paul"} };
   std::map<string, string> insertParamMap = {  {"username", "wenchy"}, {"password", "8285919"} };
   std::map<string, string> updateParamMap = {  {"username", "davey"}, {"password", "dddd"} };

   createTable();
   insert("user", insertParamMap);
   select("user", selectParamMap);
  
   //findALL("user");
   update("user", "2", updateParamMap);
   find("user", "2");
}

Database & Database::createTable()
{
   /* Create SQL statement */
   const char *sql_user =     "CREATE TABLE USER(" \
                              "ID            INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL," \
                              "USERNAME      TEXT                                NOT NULL," \
                              "PASSWORD      TEXT                                NOT NULL," \
                              "CREATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
                              "UPDATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
                              "STATE         INTEGER  DEFAULT 0 );";

   /* Execute SQL statement */
   rc = sqlite3_exec(pDb, sql_user, NULL, NULL, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "createTable: error, %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "createTable: successfully\n");
   }

   return *this;
}

bool Database::execute(const char *sql, Database * pDatabase)
{
   /* Execute SQL statement */
   rc = sqlite3_exec(pDb, sql, callback, pDatabase, &zErrMsg);
   if ( rc != SQLITE_OK ){
      fprintf(stderr, "insert: error, %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return false;
   } else {
      //fprintf(stdout, "insert: Records created successfully\n");
      //printResult("execute");
      return true;
   }
}

bool Database::insert(string tblname, map<string, string> & kvMap)
{
     /* Create SQL statement */
   sqlSring.clear();
   string valString;
   sqlSring += "INSERT INTO " + tblname + " ('";
   valString += "VALUES ('";

   map<string ,string>::iterator it=kvMap.begin();

   sqlSring += it->first;
   valString += it->second;
   for (++it; it!=kvMap.end(); ++it)
   {
      sqlSring += "', '" + it->first;
      valString += "', '" + it->second;
   }
   sqlSring += "') ";
   valString += "') ";
   sqlSring += valString;

   std::cout << "insert: " << sqlSring << std::endl;
   /* Execute SQL statement */
   resultMap.clear();
   if (execute(sqlSring.c_str(), this))
   {
      return true;
   } else {
      return false;
   }
}

bool Database::select(string tblname, map<string, string> & kvMap)
{
    /* Construct SQL statement */
   sqlSring.clear();
   sqlSring += "SELECT * from ";
   sqlSring += tblname;
   sqlSring += " WHERE ";

   map<string ,string>::iterator it=kvMap.begin();
   sqlSring += it->first + "='" + it->second + "'";
   for (++it; it!=kvMap.end(); ++it)
   {
      sqlSring += " and " + it->first + "='" + it->second + "'";
   }
   std::cout << "select: " << sqlSring << std::endl;
   /* Execute SQL statement */
   resultMap.clear();
   if (execute(sqlSring.c_str(), this))
   {
      return true;
   } else {
      return false;
   }
}


bool Database::update(string tblname, string id, map<string, string> & kvMap)
{
   /* Construct SQL statement */
   sqlSring.clear();
   sqlSring += "UPDATE "; //SET column1 = value1, column2 = value2...., columnN = valueN
   sqlSring += tblname;
   sqlSring += " SET ";

   map<string ,string>::iterator it=kvMap.begin();
   sqlSring += it->first + "='" + it->second + "'";
   for (++it; it!=kvMap.end(); ++it)
   {
      sqlSring += ", " + it->first + "='" + it->second + "'";
   }
   sqlSring += " WHERE id='" + id + "'";
   std::cout << "update: " << sqlSring << std::endl;
   /* Execute SQL statement */
   resultMap.clear();
   if (execute(sqlSring.c_str(), this))
   {
      return true;
   } else {
      return false;
   }
}

bool Database::remove()
{
   /* Create merged SQL statement */


   /* Execute SQL statement */
   resultMap.clear();
   if (execute(sqlSring.c_str(), this))
   {
      return true;
   } else {
      return false;
   }
}

bool Database::find(string tblname, string id)
{
   sqlSring.clear();
   sqlSring += "SELECT * from " + tblname + " WHERE id='";
   sqlSring += id +"'";
   std::cout << "find: " << sqlSring << std::endl;
   resultMap.clear();
   if (execute(sqlSring.c_str(), this))
   {
      return true;
   } else {
      return false;
   }
}

bool Database::findALL(string tblname)
{
   sqlSring.clear();
   sqlSring += "SELECT * from " + tblname;
   std::cout << "findALL: " << sqlSring << std::endl;
   resultMap.clear();
   if (execute(sqlSring.c_str(), this))
   {
      return true;
   } else {
      return false;
   }
}

map<string ,string>  & Database::getResult()
{
   return resultMap;
}

bool Database::first()
{

   return false;
}
void Database::printResult(string sqlAction)
{
   std::cout << sqlAction << ": \n";
   for (map<string, string>::iterator it=resultMap.begin(); it!=resultMap.end(); ++it)
         std::cout << it->first << ": " << it->second << '\n';
}
void Database::clean()
{
   
}

void Database::dump()
{
   
}

Database::~Database()
{
   sqlite3_close(pDb);
}

// int main()
// {
//    Database db(DBFILENAME);
//     db.init();
//     return 0;
// }