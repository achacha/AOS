#include "apiADatabase_SQLite.hpp"
#include "ASQLiteServer.hpp"
#include "AResultSet.hpp"
#include "AFile_IOStream.hpp"
#include "ARope.hpp"
#include "AUrl.hpp"

void testRelativeUrl()
{
  AUrl url("sqlite:///./aosconfig/AObjectServer.db");
  AString str;

  std::cout << url.getPathAndFilename() << std::endl;
}


int main()
{
  testRelativeUrl();

  return 0;
}
