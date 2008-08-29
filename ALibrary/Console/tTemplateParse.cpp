
#include "ATemplate.hpp"
#include "AFile_AString.hpp"
#include "AFile_IOStream.hpp"
#include "AEventVisitor.hpp"
#include "AXmlElement.hpp"
#include "AXmlDocument.hpp"
#include "ABasePtrContainer.hpp"

void testSimpleParse()
{
  AFile_AString strfile(
"Hello %[MODEL]{{{user/name}}}[MODEL]%!\
");

  ABasePtrContainer objects;
  AEventVisitor visitor;
  AXmlDocument model("root");
  model.useRoot().addElement(ASWNL("user/name"), ASWNL("Alex"));
  model.useRoot().addElement(ASWNL("user/loc"), ASWNL("(0,0)"));
//    doc.debugDump();
  ATemplateContext ctx(objects, model, visitor);

  //a_Read template
  ATemplate tm;
  tm.fromAFile(strfile);

  //a_Write it to cout
  AFile_IOStream iosfile;
  //std::cout << "---------------------debugDump---------------------" << std::endl;
  //tm.debugDump();
  //std::cout << "---------------------debugDump---------------------" << std::endl;
  //std::cout << "\r\n---------------------toAFile-----------------------" << std::endl;
  //tm.toAFile(iosfile);
  //std::cout << "---------------------toAFile-----------------------" << std::endl;
  std::cout << "\r\n---------------------emit-----------------------" << std::endl;
  tm.emit(iosfile);
  std::cout << "---------------------emit-----------------------" << std::endl;
  //std::cout << "\r\n---------------------emitXml-----------------------" << std::endl;
  //AXmlElement xml;
  //tm.emitXml(xml);
  //xml.emit(iosfile,0);
  //std::cout << "---------------------emitXml-----------------------" << std::endl;

  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(ctx, rope);
  std::cout << "\r\n---------------------process---------------------" << std::endl;
  rope.emit(iosfile);
  std::cout << "---------------------emit---------------------" << std::endl;
}

int main()
{
  try
  {
    testSimpleParse();
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception caught." << std::endl;
  }

  return 0;
}
