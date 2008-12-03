
#include "AOS.hpp"
#include "AXmlDocument.hpp"

#ifndef NDEBUG
#include "MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif

int main()
{
  AXmlDocument doc;
  AXmlElement &e0 = doc.useRoot().overwriteElement("foo/bar");
  AXmlElement &e1 = doc.useRoot().overwriteElement("foo/bar");
  e1.setData(ASWNL("This is a test"));

  return 0;
}