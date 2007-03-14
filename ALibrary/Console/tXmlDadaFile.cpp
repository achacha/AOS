#include <ARope.hpp>
#include <AXmlDocument.hpp>
#include <AFile_Physical.hpp>
#include <ARopeDeque.hpp>

int main()
{
  AFile_Physical configfile;
//  configfile.open("c:\\Code\\ADadaPoem\\dadadata\\poem_dada.xml");
  configfile.open("q:\\cdatatest.xml");

  AXmlDocument doc;
  doc.fromAFile(configfile);

#ifdef __DEBUG_DUMP__
  doc.debugDump();
#endif

  ARope rope;
  doc.emitXml(rope, 0);

  std::cout << rope.toAString() << std::endl;

  return 0;
}
