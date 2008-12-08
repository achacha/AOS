
#include "AOS.hpp"
#include "AXmlDocument.hpp"

#if !defined(NDEBUG) && defined(WIN32) && !defined(WIN64)
#include "MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif

int main()
{
  AXmlDocument doc;
  AXmlElement &e0 = doc.useRoot().overwriteElement("foo/bar");
  e0.setData(ASWNL("This is the first test"));
  AXmlElement &e1 = doc.useRoot().overwriteElement("foo/bar");
  e1.setData(ASWNL("This is the second test"));

  return 0;
}