#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "AString.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "ATemplateNodeHandler_CODE.hpp"

int main()
{
  AFile_AString strf(
"\
%[LUA]{{{\
local val = alibrary.Model_emitContentFromPath(\"/root/data\"); \n\
val = val + 3; \n\
val = val .. \" and more\"; \n\
alibrary.Model_addElementText(\"/newElement/item\", val); \n\
alibrary.print(alibrary.Model_emitXml(0)); \n\
}}}[LUA]%%[CODE]{{{print(/root);}}}[CODE]%\
");

  ABasePtrHolder objects;
  AXmlDocument *pdoc = new AXmlDocument(ASW("root",4));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper no swiping! 0"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper No swiping! 1"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("Swiper NO Swiping! 2"));
  pdoc->useRoot().addElement("data").addData(ASWNL("5"));
  objects.insert(ATemplate::OBJECTNAME_MODEL, pdoc, true);

  ATemplate t;
  t.addHandler(new ATemplateNodeHandler_LUA());
  t.addHandler(new ATemplateNodeHandler_CODE());

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