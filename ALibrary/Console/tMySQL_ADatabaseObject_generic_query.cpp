
#include "AMySQLServer.hpp"
#include "AFile_IOStream.hpp"
#include "ADatabaseObject.hpp"
#include "AObjectContainer.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: this <database name> <query>" << std::endl;
    std::cout << "If <query> is omitted, database metadata is dumped" << std::endl;
    return -1;
  }

  AMySQLServer server(argv[1]);
  AString strError;
  if (!server.init(strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }
  
  AString strQuery("select * from item where id=2");
  ADatabaseObject qr("myquery");
  if (!server.executeSQL(qr, strQuery, strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }

  AElement element;
  AString result;
  qr.publish(&element);
  element.asXml(result);
  std::cout << result << std::endl;

  std::cout << "Only row 0" << std::endl;
  result.clear();
  const AObjectContainer& ns = qr.getRow(0);
  std::cout <<
    "id=" <<
    ns.getAs<AString>("id") <<
    " name=" <<
    ns.getAs<AString>("name") << 
    " weight=" << 
    ns.getAs<AString>("creator") << 
    std::endl;

  return 0;
}
