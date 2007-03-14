#include "AUrl.hpp"
#include "AMySQLServer.hpp"
#include "AResultSet.hpp"
#include "AXmlElement.hpp"
#include "ARope.hpp"

int main(int argc, char* argv[])
{
  AUrl dbUrl("mysql://aos:aos@localhost/aos/");
  AString strError;
  AMySQLServer server(dbUrl);
  if (!server.init(strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }
  
  AResultSet resultSet;
  if (server.executeSQL("SELECT * FROM global", resultSet, strError))
  {
    resultSet.debugDump();
    
    AXmlElement element("query");
    element.addAttribute("sql", "SELECT * FROM global");
    resultSet.emit(element);

    ARope rope;
    element.emitXml(rope, 0);

    std::cout << rope.toAString() << std::endl;
  }
  else
    std::cout << strError << std::endl;
  
  return 0;
}
