#include "ATemplate.hpp"
#include "AFile_AString.hpp"
#include "AFile_IOStream.hpp"
#include "AEventVisitor.hpp"
#include "AString.hpp"
#include "AUrl.hpp"
#include "AXmlElement.hpp"
#include "ANumber.hpp"

void testSimpleParse()
{
  AFile_AString strfile("\
Hello %%CODE%%{\r\n print(/root/user/name); print(/root/user/loc); \r\n}%%CODE%%!\
");

  AXmlElement ns("root");
  ns.addElement("/user/name", "Alex");
  ns.addElement("/user/loc", "(0,0)");
//    ns.debugDump();

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
  tm.process(rope, ns);
  std::cout << "\r\n---------------------process---------------------" << std::endl;
  rope.toAFile(iosfile);
  std::cout << "---------------------process---------------------" << std::endl;
}

void testTemplateParse()
{
  AFile_AString strfile("\
Hello %%CODE%%{\r\n print(/root/user/name); print(/root/user/loc); \r\n}%%CODE%%, welcome to %%CODE%%{\t print(/root/user/url);  }%%CODE%%!!11!!1\r\n\
print='%%CODE%%{print(/root/user/cpu)}%%CODE%%'\r\n\
count='%%CODE%%{count(/root/user/cpu)}%%CODE%%'\r\n\
");

  AXmlElement ns("root");
  ns.addElement("/user/name", "Alex");
  ns.addElement("/user/loc", "(0,0)");
  ns.addElement("/user/cpu", "iX86");
  ns.addElement("/user/cpu", "x68000");
  ns.addElement("/user/cpu", "x6502");
  ns.addElement("/user/url", "http://www.achacha.org:8888/home/index.html");
//    ns.debugDump();

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
  tm.process(rope, ns);
  std::cout << "\r\n---------------------process---------------------" << std::endl;
  rope.toAFile(iosfile);
  std::cout << "---------------------process---------------------" << std::endl;
}

void testHtmlTemplate()
{
  AFile_AString strfile("<html>\
<body>\
%%CODE%%{print(/root)}%%CODE%%<br/>\
</body>\
</html>");

  AXmlElement eRoot("root");
  eRoot.addElement("/user/name", "Alex");
  eRoot.addElement("/user/cpu", "iX86");

  ATemplate tm;
  tm.fromAFile(strfile);

  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(rope, eRoot);
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
