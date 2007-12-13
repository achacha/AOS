#include "ATemplate.hpp"
#include "ATemplateNodehandler_CODE.hpp"
#include "ATemplateNodehandler_OBJECT.hpp"
#include "ATemplateNodehandler_MODEL.hpp"
#include "AFile_AString.hpp"
#include "AFile_IOStream.hpp"
#include "AEventVisitor.hpp"
#include "AString.hpp"
#include "AUrl.hpp"
#include "AXmlElement.hpp"
#include "AXmlDocument.hpp"
#include "ANumber.hpp"

void testSimpleParse()
{
  AFile_AString strfile(
"Hello %[CODE]{{{\r\n print(/root/user/name); \r\n print(/root/user/loc); \r\n}}}[CODE]%!\
");

  ABasePtrHolder objects;
  AXmlDocument *pDoc = new AXmlDocument("root");
  pDoc->useRoot().addElement(ASWNL("user/name"), ASWNL("Alex"));
  pDoc->useRoot().addElement(ASWNL("user/loc"), ASWNL("(0,0)"));
//    doc.debugDump();
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);

  //a_Read template
  ATemplate tm;
  tm.fromAFile(strfile);

  //a_Write it to cout
  AFile_IOStream iosfile;
  std::cout << "---------------------debugDump---------------------" << std::endl;
  tm.debugDump();
  std::cout << "---------------------debugDump---------------------" << std::endl;
  std::cout << "\r\n---------------------toAFile-----------------------" << std::endl;
  tm.toAFile(iosfile);
  std::cout << "---------------------toAFile-----------------------" << std::endl;
  std::cout << "\r\n---------------------emit-----------------------" << std::endl;
  tm.emit(iosfile);
  std::cout << "---------------------emit-----------------------" << std::endl;
  std::cout << "\r\n---------------------emitXml-----------------------" << std::endl;
  AXmlElement xml;
  tm.emitXml(xml);
  xml.emit(iosfile,0);
  std::cout << "---------------------emitXml-----------------------" << std::endl;

  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(objects, rope);
  std::cout << "\r\n---------------------process---------------------" << std::endl;
  rope.toAFile(iosfile);
  std::cout << "---------------------process---------------------" << std::endl;
}

void testTemplateParse()
{
  AFile_AString strfile(
    "Hello %[CODE]{{{\r\n print(/root/user/name); \r\n print(/root/user/loc); \r\n}}}[CODE]%, welcome to %[CODE]{{{\t print(/root/user/url);  }}}[CODE]%!!11!!1\r\n\
    print='%[CODE]{{{print(/root/user/cpu)}}}[CODE]%'\r\n\
    count='%[CODE]{{{count(/root/user/cpu)}}}[CODE]%'\r\n\
");

  ABasePtrHolder objects;
  AXmlDocument *pDoc = new AXmlDocument("root");
  pDoc->useRoot().addElement(ASWNL("user/name"), ASWNL("Alex"));
  pDoc->useRoot().addElement(ASWNL("user/loc"), ASWNL("(0,0)"));
  pDoc->useRoot().addElement(ASWNL("user/cpu"), ASWNL("iX86"));
  pDoc->useRoot().addElement(ASWNL("user/cpu"), ASWNL("x68000"));
  pDoc->useRoot().addElement(ASWNL("user/cpu"), ASWNL("x6502"));
  pDoc->useRoot().addElement(ASWNL("user/url"), ASWNL("http://www.achacha.org:8888/home/index.html"));
//    ns.debugDump();
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);

  //a_Read template
  ATemplate tm;
  tm.fromAFile(strfile);

  //a_Write it to cout
  AFile_IOStream iosfile;
  std::cout << "---------------------debugDump---------------------" << std::endl;
  tm.debugDump();
  std::cout << "\r\n---------------------debugDump---------------------" << std::endl;
  std::cout << "---------------------toAFile-----------------------" << std::endl;
  tm.toAFile(iosfile);
  std::cout << "\r\n---------------------toAFile-----------------------" << std::endl;
  std::cout << "---------------------emit-----------------------" << std::endl;
  tm.emit(iosfile);
  std::cout << "\r\n---------------------emit-----------------------" << std::endl;
  std::cout << "---------------------emitXml-----------------------" << std::endl;
  AXmlElement xml;
  tm.emitXml(xml);
  xml.emit(iosfile,0);
  std::cout << "\r\n---------------------emitXml-----------------------" << std::endl;

  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(objects, rope);
  std::cout << "---------------------process---------------------" << std::endl;
  rope.emit(iosfile);
  std::cout << "\r\n---------------------process---------------------" << std::endl;
}

void testHtmlTemplate()
{
  AFile_AString strfile("<html>\
<body>\
%[CODE]{{{print(/root)}}}[CODE]%<br/>\
</body>\
</html>");

  ABasePtrHolder objects;
  AXmlDocument *pDoc = new AXmlDocument("root");
  pDoc->useRoot().addElement(ASWNL("user/name"), ASWNL("Alex"));
  pDoc->useRoot().addElement(ASWNL("user/cpu"), ASWNL("iX86"));
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);

  ATemplate tm;
  tm.fromAFile(strfile);
  
  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(objects, rope);
  std::cout << "\r\n\r\n" << rope << std::endl;
}

void testObjectAndModel()
{
  AFile_AString strfile("\
print(/root)=%[CODE]{{{print(/root)}}}[CODE]%\r\n\
object[someobject]=%[OBJECT]{{{someobject}}}[OBJECT]%\r\n\
model[/root/user/cpu]=%[MODEL]{{{/root/user/cpu}}}[MODEL]%\r\n\
</html>");

  //a_Prepare objects and model
  ABasePtrHolder objects;
  AXmlDocument *pDoc = new AXmlDocument("root");
  pDoc->useRoot().addElement(ASWNL("user/name")).addData(ASWNL("Alex"));
  pDoc->useRoot().overwriteElement(ASWNL("user/cpu")).addData(ASWNL("iX86"));
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);
  objects.insert(ASWNL("someobject"), new AString("somestringvalue"), true);

  pDoc->debugDump();

  //a_Prepare template with nodes to support
  ATemplate tm;

  //a_Read the template
  tm.fromAFile(strfile);
  tm.debugDump();

  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(objects, rope);
  std::cout << "\r\n\r\n" << rope << std::endl;
}

int main()
{
  try
  {
    testSimpleParse();
    testTemplateParse();
    testHtmlTemplate();
    testObjectAndModel();
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
