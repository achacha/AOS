
#include "AUrl.hpp"
#include "AMySQLServer.hpp"
#include "AString.hpp"
#include "AResultSet.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"

int main(int argc, char **argv)
{
  AUrl url("mysql://aos:aos@localhost/aos/");
  AMySQLServer server(url);
  AString strError;
  if (!server.init(strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }
  else
  {
    ARope rope;
    AXmlElement base("base");
    server.emit(base);

    AResultSet rs;
    if (server.executeSQL("SELECT * FROM global", rs, strError))
    {
      rs.emit(base);
    }
    else
      base.addElement("error", strError);

    base.emitXml(rope, 0);
    std::cout << rope << std::endl;
  }
  
  return 0;
}
