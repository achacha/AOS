
#include "AMySQLServer.hpp"
#include "AFile_IOStream.hpp"
#include "AObjectContainer.hpp"
#include "ARope.hpp"

int main(int, char**)
{
  AMySQLServer server("bilopia");
  
  AString strError;
//  if (!server.init(strError))
//  {
//    std::cout << strError << std::endl;
//    return -1;
//  }
  
  AObjectContainer ns;
  ns.insert("uname", "Alex");
  ns.insert("uplace", "here");
  ns.insert("data", "a=1&b=2&c=3");

  if (!server.save("item", AString("1"), ns, strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }

  ns.debugDump();
  ns.clear();

  if (!server.load("item", "1", ns, strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }
  ns.debugDump();

  //a_XML dump
  AXmlElement element("root");
  ARope rope;
  ns.publish(element);
  element.emitXml(rope, 0);
  element.debugDump();
  std::cout << rope.toString() << std::endl;

  return 0;
}
