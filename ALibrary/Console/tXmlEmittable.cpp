#include "AUrl.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"

void testAUrl()
{
  AUrl url("http://someuser:somepass@someservername.com:8188/path0/path1/filename.ext?param0=1&param2&param3=foo");
  AXmlElement element("base");

  ARope rope;
  url.emit(element);
  element.emitXml(rope);
  std::cout << rope.toAString() << std::endl;
}

int main()
{
  testAUrl();

  return 0;
}

