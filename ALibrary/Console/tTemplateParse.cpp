#include "ATemplate.hpp"
#include "AFile_AString.hpp"
#include "AFile_IOStream.hpp"
#include "AEventVisitor.hpp"
#include "AString.hpp"
#include "AUrl.hpp"
#include "AXmlElement.hpp"
#include "ANumber.hpp"

void testTemplateParse()
{
  AFile_AString strfile("\
Hello <!--[\r\n print(/root/user/name); print(/root/user/loc); \r\n]-->, welcome to <!--[\t print(/root/user/url);  ]-->!!11!!1\r\n\
print='<!--[print(/root/user/cpu)]-->'\r\n\
count='<!--[count(/root/user/cpu)]-->'\r\n\
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
//    tm.debugDump();
//  tm.toAFile(iosfile);

  //a_Evaluate and emit to cout
  ARope rope;
  tm.process(rope, ns);
  std::cout << "\r\n\r\n";
  rope.toAFile(iosfile);
}

void testHtmlTemplate()
{
  AFile_AString strfile("<html>\
<body>\
<!--[print(/root)]--><br/>\
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
    //testTemplateParse();
    testHtmlTemplate();
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
