#include "AObjectContainer.hpp"
#include "ARope.hpp"
#include "AFile_AString.hpp"
#include "templateAutoPtr.hpp"
#include "AMySQLServer.hpp"
#include "ATextGenerator.hpp"
#include "AXmlDocument.hpp"

int main()
{
  AObjectContainer ns;
  ns.insert("/path/foo", "bar");
  ns.insert("/path/foo/bar", "baz");

  ns.debugDump();

  return 0;
}


