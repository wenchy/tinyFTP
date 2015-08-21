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
   if (pDatabase != NULL)
   {
      vector< map<string ,string> > & resultMapVector = ((Database *)pDatabase)->getResult();
      map<string, string> kvMap;
      for(int i=0; i<argc; i++){
         //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
         kvMap.insert( std::pair<string, string>(azColName[i], argv[i]) );
      }
      resultMapVector.push_back(kvMap);
   }
   return 0;
   
}

Database::Database(const char * zDbFilename): dbFilename(zDbFilename)
{

   //create tinyFTP root working diretory

   DIR* d = opendir(ROOTDIR);
   if(d)
   {  
      fprintf(stderr, "Already exists: %s\n",  ROOTDIR);
      closedir(d);
   } else if(mkdir(ROOTDIR, 0777) == -1)
   {
      char buf[MAXLINE];
      fprintf(stdout, "Error(%s): %s\n", ROOTDIR, strerror_r(errno, buf, MAXLINE));
   } else {
      fprintf(stdout, "Directory created: %s\n", ROOTDIR);
   }

   // create tinyFTP/.tinyFTP/ working diretory
   d = opendir(KERNELDIR);
   if(d)
   {  
      fprintf(stderr, "Already exists: %s\n",  KERNELDIR);
      closedir(d);
   } else if(mkdir(KERNELDIR, 0777) == -1)
   {
      char buf[MAXLINE];
      fprintf(stdout, "Error(%s): %s\n", KERNELDIR, strerror_r(errno, buf, MAXLINE));
   } else {
      fprintf(stdout, "Directory created: %s\n", KERNELDIR);
   }
   
   // create tinyFTP/.tinyFTP/ghost/ working diretory
   d = opendir(GHOSTDIR);
   if(d)
   {  
      fprintf(stderr, "Already exists: %s\n",  GHOSTDIR);
      closedir(d);
   } else if(mkdir(GHOSTDIR, 0777) == -1)
   {
      char buf[MAXLINE];
      fprintf(stdout, "Error(%s): %s\n", GHOSTDIR, strerror_r(errno, buf, MAXLINE));
   } else {
      fprintf(stdout, "Directory created: %s\n", GHOSTDIR);
   }

    

    //clean();
    zErrMsg = NULL;
    /* Open database */
    string dirString = KERNELDIR;
    rc = sqlite3_open((dirString + dbFilename).c_str(), &pDb);
    if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(pDb));
    exit(0);
    }else{
       fprintf(stdout, "Open database successfully\n");
    }
}

void Database::traverseFiles(string dirpath)
{
   DIR * dir= opendir(dirpath.c_str());

   if(!dir)
   {
      Error::ret("opendir");
      return;
   }

   if (dirpath.back() != '/')
   {
      dirpath += "/";
   }
   struct dirent* e;
   while( (e = readdir(dir)) )
   {
      if(e->d_type == 4 && strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
      {
         traverseFiles(dirpath + e->d_name);
      }
      else if(e->d_type == 8)
      {
         string filepath = dirpath + e->d_name;     
         cout << "\n\nmd5sum: " << filepath << " ..." << endl;
         string md5str = md5sum(filepath.c_str());
         string sizestr = getFilesize(filepath);
         cout << "filepath: " << filepath << "md5str: " << md5str << "sizestr: " << sizestr << endl;

         if (!md5str.empty() && !sizestr.empty())
         {
            std::map<string, string> selectParamMap = {  {"MD5SUM", md5str} };
            if (select("file", selectParamMap))
            {
               vector< map<string ,string> > resultMapVector = getResult();
               if (resultMapVector.empty())
               {
                  std::map<string, string> insertParamMap = {  {"MD5SUM", md5str},
                                                               {"MD5RAND", "NULL"},
                                                               {"ABSPATH", filepath},
                                                               {"SIZE", sizestr} };
                  insert("file", insertParamMap);

               } else {
                  Error::msg("\033[31mMD5SUM already exist\033[0m");
                  printResult();
               }
            } else {
               Error::msg("\033[31mDatabase select error\033[0m");
            }   
         }   
      }
   }
   closedir(dir);
}

// void Database::init1(const char * zDbFilename)
// {
//    //clean();
//    dbFilename = zDbFilename;
//    zErrMsg = NULL;
//    /* Open database */
//    rc = sqlite3_open(dbFilename.c_str(), &pDb);
//    if( rc ){
//     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(pDb));
//     exit(0);
//  }else{
//     fprintf(stdout, "Open database successfully\n");
//  }
// }

void Database::init()
{
   std::map<string, string> insertParamMap0 = { {"id", "1"}, 
                                                {"username", "admin"},
                                                {"password", encryptPassword("admin")} };
   std::map<string, string> insertParamMap1 = { {"id", "2"}, 
                                                {"username", "anonymous"},
                                                {"password", encryptPassword("anonymous")} };
   std::map<string, string> insertParamMap2 = { {"id", "3"}, 
                                                {"username", "charles"},
                                                {"password", encryptPassword("charles")} };
   std::map<string, string> insertParamMap3 = { {"id", "4"}, 
                                                {"username", "davey"},
                                                {"password", encryptPassword("davey")} };

   std::map<string, string> selectParamMap = {  {"id", "1"}, {"username", "Paul"} };
   std::map<string, string> updateParamMap = {  {"username", "davey"}, {"password", "dddd"} };

   create();
   insert("user", insertParamMap0);
   insert("user", insertParamMap1);
   insert("user", insertParamMap2);
   insert("user", insertParamMap3);
   //select("user", selectParamMap);

   // init user's root working directory
   if(findALL("user"))
   {
      vector< map<string ,string> > myresultMapVector;
      getResult(myresultMapVector);
      for (vector< map<string ,string> >::iterator iter=myresultMapVector.begin(); iter!=myresultMapVector.end(); ++iter)
      {
         string dirString(ROOTDIR);
         dirString += (*iter)["USERNAME"];
         DIR* d = opendir(dirString.c_str());
         if(d)
         {  
            fprintf(stderr, "Already exists: %s\n",  dirString.c_str());
            closedir(d);
         }else if(mkdir(dirString.c_str(), 0777) == -1)
         {
            char buf[MAXLINE];
            fprintf(stdout, "Error(%s): %s\n", dirString.c_str(), strerror_r(errno, buf, MAXLINE));
         } else {
            fprintf(stdout, "Directory created: %s\n", dirString.c_str());
         }

         //traverseFiles(dirString);
      }
   }

   

   //update("user", "2", updateParamMap);
   //find("user", "2");
}

Database & Database::create()
{
   std::cout << "Database::create" << std::endl;

   /* Create SQL statement */
   const char *sql_table_user =  "CREATE TABLE USER(" \
      "ID            INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL," \
      "USERNAME      TEXT UNIQUE                         NOT NULL," \
      "PASSWORD      TEXT                                NOT NULL," \
      "RCWD          TEXT DEFAULT '/', " \
      "CREATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "UPDATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "STATE         INTEGER  DEFAULT 0 );";

   const char *sql_table_file =  "CREATE TABLE FILE(" \
      "ID            INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL," \
      "MD5SUM        TEXT UNIQUE                         NOT NULL," \
      "MD5RAND       TEXT                                NOT NULL," \
      "ABSPATH       TEXT                                NOT NULL," \
      "SIZE          INTEGER                             NOT NULL," \
      "CREATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "UPDATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "VALID         INTEGER  DEFAULT 1 ," \
      "ACCESS        INTEGER  DEFAULT 0 );";
   
   // interrupted file
   const char *sql_table_ifile =  "CREATE TABLE IFILE(" \
      "ID            INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL," \
      "USERID        TEXT                                NOT NULL," \
      "ABSPATH       TEXT                                NOT NULL," \
      "FILENAME      TEXT                                NOT NULL," \
      "SIZE          INTEGER                             NOT NULL," \
      "MD5SUM        TEXT                                NOT NULL," \
      "NSLICE        INTEGER                             NOT NULL," \
      "SINDEX        INTEGER                             NOT NULL," \
      "SLICECAP      INTEGER                             NOT NULL," \
      "CREATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "UPDATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "VALID         INTEGER  DEFAULT 1 );";

   /* Execute SQL statement */
   execute(sql_table_user, NULL);
   execute(sql_table_file, NULL);
   execute(sql_table_ifile, NULL);

return *this;
}

Database & Database::createTable()
{
   /* Create SQL statement */
   const char *sql_user =     "CREATE TABLE USER(" \
      "ID            INTEGER PRIMARY KEY AUTOINCREMENT   NOT NULL," \
      "USERNAME      TEXT UNIQUE                         NOT NULL," \
      "PASSWORD      TEXT                                NOT NULL," \
      "CREATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "UPDATE_AT     DATETIME DEFAULT (datetime('now', 'localtime'))," \
      "STATE         INTEGER  DEFAULT 0 );";

    /* Execute SQL statement */
   execute(sql_user, NULL);

return *this;
}

bool Database::execute(const char *sql, Database * pDatabase)
{
   /* Clear resultMap */
   resultMapVector.clear();
   /* Execute SQL statement */
   rc = sqlite3_exec(pDb, sql, callback, pDatabase, &zErrMsg);
   if ( rc != SQLITE_OK ){
      fprintf(stderr, "\033[31mDatabase execute error: %s\033[0m\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return false;
   } else {
      fprintf(stdout, "\033[32mDatabase execute successfully\033[0m\n");
      printResult();
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
      /* Execute SQL statement and return*/
   return execute(sqlSring.c_str(), this);
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
   std::cout << "query: " << sqlSring << std::endl;
   /* Execute SQL statement */
   return execute(sqlSring.c_str(), this);
}


bool Database::selectNewest(string tblname, map<string, string> & kvMap)
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
   sqlSring += " order by ID desc";
   std::cout << "query: " << sqlSring << std::endl;
   /* Execute SQL statement */
   return execute(sqlSring.c_str(), this);
}



bool Database::update(string tblname, string id, map<string, string> & kvMap)
{
   /* Construct SQL statement */
   sqlSring.clear();
   sqlSring += "UPDATE ";
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
   return execute(sqlSring.c_str(), this);
}

bool Database::update(string tblname, map<string, string> & whereMap, map<string, string> & kvMap)
{
   /* Construct SQL statement */
   sqlSring.clear();
   sqlSring += "UPDATE ";
   sqlSring += tblname;
   sqlSring += " SET ";

   map<string ,string>::iterator it=kvMap.begin();
   sqlSring += it->first + "='" + it->second + "'";
   for (++it; it!=kvMap.end(); ++it)
   {
      sqlSring += ", " + it->first + "='" + it->second + "'";
   }
   sqlSring += " WHERE ";

   map<string ,string>::iterator iter=whereMap.begin();
   sqlSring += iter->first + "='" + iter->second + "'";
   for (++iter; iter!=whereMap.end(); ++iter)
   {
      sqlSring += " and " + iter->first + "='" + iter->second + "'";
   }
   std::cout << "update: " << sqlSring << std::endl;
   /* Execute SQL statement */
   return execute(sqlSring.c_str(), this);
}

bool Database::remove(string tblname, string id)
{
   /* Create merged SQL statement */
   sqlSring.clear();
   sqlSring += "DELETE FROM "; //SET column1 = value1, column2 = value2...., columnN = valueN
   sqlSring += tblname;
   sqlSring += " WHERE id='" + id + "'";

   /* Execute SQL statement */
   return execute(sqlSring.c_str(), this);
}

bool Database::remove(string tblname, map<string, string> & kvMap)
{
   /* Create merged SQL statement */
   sqlSring.clear();
   sqlSring += "DELETE FROM "; //SET column1 = value1, column2 = value2...., columnN = valueN
   sqlSring += tblname;
   sqlSring += " WHERE ";

   map<string ,string>::iterator it=kvMap.begin();
   sqlSring += it->first + "='" + it->second + "'";
   for (++it; it!=kvMap.end(); ++it)
   {
      sqlSring += " and " + it->first + "='" + it->second + "'";
   }
   std::cout << "remove: " << sqlSring << std::endl;
   /* Execute SQL statement */
   return execute(sqlSring.c_str(), this);
}

bool Database::find(string tblname, string id)
{
   sqlSring.clear();
   sqlSring += "SELECT * from " + tblname + " WHERE id='";
   sqlSring += id +"'";
   std::cout << "find: " << sqlSring << std::endl;
   return execute(sqlSring.c_str(), this);
}

bool Database::findALL(string tblname)
{
   sqlSring.clear();
   sqlSring += "SELECT * from " + tblname;
   std::cout << "findALL: " << sqlSring << std::endl;
   return execute(sqlSring.c_str(), this);
}

void Database::getResult(vector< map<string ,string> >  & resultMapVector_o)
{
   resultMapVector_o.swap(this->resultMapVector);
}

vector< map<string ,string> >  & Database::getResult()
{

   return this->resultMapVector;      

}

bool Database::first()
{

   return false;
}
void Database::printResult()
{
   for (vector< map<string ,string> >::iterator iter=resultMapVector.begin(); iter!=resultMapVector.end(); ++iter)
   {
       for (map<string, string>::iterator it=iter->begin(); it!=iter->end(); ++it)
            std::cout << it->first << ": " << it->second << '\n';
       std::cout << '\n';
   }
}
void Database::clean()
{
   if( unlink(dbFilename.c_str()) !=0 )
   {
      char buf[MAXLINE];
      fprintf(stderr, "DB clean error: %s\n", strerror_r(errno, buf, MAXLINE));
      
   } else {
      fprintf(stderr, "DB cleaned\n");
   }
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