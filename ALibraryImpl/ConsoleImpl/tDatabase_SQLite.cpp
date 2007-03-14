#include "apiADatabase_SQLite.hpp"
#include "ASQLiteServer.hpp"
#include "AResultSet.hpp"
#include "AFile_IOStream.hpp"
#include "ARope.hpp"

u4 createTable(ADatabase& db, AString& error)
{
  AResultSet rs;
AString query(
"CREATE TABLE `global` (\
  `id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,\
  `name` varchar(45) NOT NULL default '',\
  `value` varchar(1024) NOT NULL default ''\
);");

  return db.executeSQL(query, rs, error);
}

int main()
{
  AUrl url("sqlite://c:/tmp/mysqlite.db");
  ASQLiteServer db(url);
  AString error;

  if (!db.init(error))
  {
    std::cerr << error << std::endl;
    return -1;
  }
  
  AString input;
  AFile_IOStream iof;
  std::cout << "\r\nsqlite>" << std::flush;
  while(AString::npos != iof.readLine(input))
  {
    if (input.equals("createTable"))
    {
      if (AString::npos == createTable(db, error))
      {
        std::cerr << error << std::endl;
        return -1;
      }
    }
    else if (input.equals("exit"))
    {
      return 0;
    }
    else
    {
      AResultSet rs;
      u4 rows = db.executeSQL(input, rs, error);
      if (AString::npos == rows)
      {
        std::cerr << error << std::endl;
        error.clear();
      }
      else
      {
        std::cout << "Success, rows affected=" << rows << std::endl;
        ARope rope;
        rs.emit(rope);
        std::cout << rope << std::endl;
      }
    }
    input.clear();
    std::cout << "\r\nsqlite>" << std::flush;
  }

  return 0;
}
