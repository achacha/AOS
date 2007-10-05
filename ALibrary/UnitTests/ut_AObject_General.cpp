#include "pchUnitTests.hpp"

#include "AObjectContainer.hpp"
#include "AXmlElement.hpp"
#include "ANumber.hpp"
#include "AUrl.hpp"
#include "ARope.hpp"

int utAObjectUrl()
{
  int iRet = 0x0;
  
  ARope rope;
  AObject<AString> objStr("Somename", "myname");
  AObjectContainer ns("root");
  ns.insert("/path0/object0", new AObject<AString>("value0"));
  ns.insert("/path1/object0", new AObject<ANumber>(ANumber("3.14")));
  ns.insert("/path0/object1", new AObject<AUrl>(AUrl("http://www.achacha.org/")));
  AXmlElement element;
  ns.emitXml(element);
  element.emit(rope);
//  std::cout << rope << std::endl;
  if (rope.toAString().compare("<root><path0><object0>value0</object0><object1>http://www.achacha.org/</object1></path0><path1><object0>3.14</object0></path1></root>"))
  {
    iRet++;
    std::cerr << "publish from AObjectContainer to AXmlElement and asXml failed." << std::endl;
  } else std::cerr << "." << std::flush;

  return iRet;
}

int ut_AObject_General()
{
  std::cerr << "ut_AObject_General" << std::endl;
  int iRet = 0;

  iRet += utAObjectUrl();

  return iRet;
}
