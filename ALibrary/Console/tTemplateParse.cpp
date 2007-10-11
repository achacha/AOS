#include "ATemplate.hpp"
#include "ATemplateNodehandler_CODE.hpp"
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
  pDoc->useRoot().addElement("/user/name", "Alex");
  pDoc->useRoot().addElement("/user/loc", "(0,0)");
//    doc.debugDump();
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);

  //a_Read template
  ATemplate tm;
  tm.addHandler(new ATemplateNodeHandler_CODE());
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
  pDoc->useRoot().addElement("/user/name", "Alex");
  pDoc->useRoot().addElement("/user/loc", "(0,0)");
  pDoc->useRoot().addElement("/user/cpu", "iX86");
  pDoc->useRoot().addElement("/user/cpu", "x68000");
  pDoc->useRoot().addElement("/user/cpu", "x6502");
  pDoc->useRoot().addElement("/user/url", "http://www.achacha.org:8888/home/index.html");
//    ns.debugDump();
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);

  //a_Read template
  ATemplate tm;
  tm.addHandler(new ATemplateNodeHandler_CODE());
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
  pDoc->useRoot().addElement("/user/name", "Alex");
  pDoc->useRoot().addElement("/user/cpu", "iX86");
  objects.insert(ATemplate::OBJECTNAME_MODEL, pDoc, true);

  ATemplate tm;
  tm.addHandler(new ATemplateNodeHandler_CODE());
  tm.fromAFile(strfile);
  
  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(objects, rope);
  std::cout << "\r\n\r\n" << rope << std::endl;
}

int main()
{
  try
  {
    //testSimpleParse();
    testTemplateParse();
    //testHtmlTemplate();
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
