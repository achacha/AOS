#include "AOSDatabaseConnectionPool.hpp"
#include "AObjectContainer.hpp"
#include "AObjectDatabase.hpp"
#include "AResultSet.hpp"
#include "ALog_AFile.hpp"
#include "AMutex.hpp"

int main()
{
  AObjectContainer global("global");
  ALog_AFile alog(new AMutex("AOSTest.log"), "AOSTest.log");

  AUrl urlServer("mysql://aos:aos@localhost/aos/");
#ifdef __DEBUG_DUMP__
  urlServer.debugDump();
#endif

  AString strError;
  AOSDatabaseConnectionPool dbp(alog);
  if (!dbp.init(urlServer, 1, strError))
  {
    std::cout << "Unable to initialize" << std::endl;
  }

  AResultSet resultSet;
  if (dbp.useDB().executeSQL("select * from global", resultSet, strError))
  {
    resultSet.emit(strError);
  }

  std::cout << strError << std::endl;

  return 0;
}