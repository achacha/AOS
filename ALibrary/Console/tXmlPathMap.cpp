#include <templateAPathMap.hpp>
#include "AAttributes.hpp"
#include "AObject.hpp"
#include "AObject_Url.hpp"

void testAObject()
{
  AObject obj("MyObject");
  obj.useAttributes().insert("attr0", "value001");
  obj.useAttributes().insert("attr1", "value010");
  obj.useAttributes().insert("attr2", "value100");
  obj.useData() = "My object's data";

  obj.debugDump(std::cout, 0);
  std::cout << std::endl << obj.toXml(0) << std::endl;
}

void testAObjectUrl()
{
  AObject_Url obj("MyObject_Url");
  obj.useAttributes().insert("attr0", "value001");
  obj.useAttributes().insert("attr1", "value010");
  obj.useAttributes().insert("attr2", "value100");
  obj.useData() = "My object's data";

  obj.debugDump(std::cout, 0);
  std::cout << std::endl << obj.toXml(0) << std::endl;
}

int main()
{
//  testAObject();
  testAObjectUrl();

  return 0x0;
}
