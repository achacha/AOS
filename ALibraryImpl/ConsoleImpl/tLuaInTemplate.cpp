#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "AString.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ABasePtrContainer.hpp"
#include "ATemplate.hpp"
#include "ATemplateNodeHandler_LUA.hpp"

int main()
{
  AFile_AString strf(
"\
%[LUA]{{{\
local val = model.getText(\"/root/data\"); \n\
val = val + 3; \n\
val = val .. \" and more\"; \n\
model.setText(\"newElement/item0\", val); \n\
model.setText(\"newElement/item1\", val); \n\
print(model.emitXml(0)); \n\
}}}[LUA]%\r\n\
\r\ncode[print(/root/newElement)]=%[CODE]{{{print(/root/newElement);}}}[CODE]%\
\r\nobject[rootelem]=%[OBJECT]{{{rootelem}}}[OBJECT]%\
\r\nmodel[/root/newElement]=%[MODEL]{{{/root/newElement}}}[MODEL]%\
\r\nmodel[/root/newElement/item0]=%[MODEL]{{{/root/newElement/item0}}}[MODEL]%\
\r\nmodel[/root/newElement/item1]=%[MODEL]{{{/root/newElement/item1}}}[MODEL]%\
");

  ABasePtrContainer objects;
  AXmlDocument *pdoc = new AXmlDocument(ASW("root",4));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper no swiping! 0"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper No swiping! 1"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("Swiper NO Swiping! 2"));
  pdoc->useRoot().addElement("data").addData(ASWNL("5"));
  objects.insert(ATemplate::OBJECTNAME_MODEL, pdoc, true);

  ATemplate t;
  t.addHandler(new ATemplateNodeHandler_LUA());

  {
    AXmlElement *pElement = new AXmlElement(ASW("root",4));
    pElement->addElement("sub0").addAttribute("attr", "zero");
    objects.insert("rootelem", new AXmlElement(*pElement), true);
  }
  
  objects.insert("simplestring", new AString("Simple string I added"), true);
  
  t.fromAFile(strf);
  std::cout << "\r\n------------------------------------ debugDump -----------------------------------------" << std::endl;
  t.debugDump(); 
  std::cout << "\r\n------------------------------------ debugDump -----------------------------------------" << std::endl;
  
  ARope rope;
  try
  {
    t.process(objects, rope);
  }
  catch(AException& ex)
  {
    std::cout << ex << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }

  std::cout << "\r\n------------------------------------ output -----------------------------------------" << std::endl;
  std::cout << rope << std::endl;
  std::cout << "\r\n------------------------------------ output -----------------------------------------" << std::endl;

  return 1;
}