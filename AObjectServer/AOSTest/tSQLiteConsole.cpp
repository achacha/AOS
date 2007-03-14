#include "apiADatabase_SQLite.hpp"
#include "ASQLiteServer.hpp"
#include "AResultSet.hpp"
#include "AFile_IOStream.hpp"
#include "ARope.hpp"

u4 createAOSTables(ADatabase& db, AString& error)
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

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: this [SQLite database URL]" << std::endl;
    std::cout << "e.g. this sqlite://q:/mydata.db" << std::endl;
    return -1;
  }
  
  AUrl url(argv[1]);
  ASQLiteServer db(url);
  AString error;

  if (!db.init(error))
  {
    std::cerr << error << std::endl;
    return -1;
  }
  
  AString input;
  AFile_IOStream iof;
  std::cout << "\r\nsqlite [?=help]>" << std::flush;
  while(AString::npos != iof.readLine(input))
  {
    if (input.equals("createAOSTables"))
    {
      if (AString::npos == createAOSTables(db, error))
        std::cerr << error << std::endl;
      else
        std::cout << "AOS tables created." << std::endl;
    }
    else if (input.equals("?"))
    {
      std::cout << "Enter SQL query or use one of the following built in functions:\r\n";
      std::cout << "  createAOSTables - create databases used by base AOS server modules\r\n";
      std::cout << std::endl;
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
