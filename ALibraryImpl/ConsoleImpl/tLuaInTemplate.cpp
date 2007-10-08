#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "AString.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "ATemplateNode_Lua.hpp"
#include "ATemplateNode_Code.hpp"

int main()
{
//  AFile_AString strf(
//"\
//#%LUA%#{\
//alibrary.print(alibrary.emit(\"simplestring\")); \n\
//alibrary.print(alibrary.Model_emitXml(0)); \n\
//alibrary.print(alibrary.Model_emitJson(0)); \n\
//alibrary.Model_addElementText(\"/newElement/item\",\"somevalue\"); \n\
//alibrary.print(alibrary.Model_emitXml(0)); \n\
//}#%LUA%#\
//");

  AFile_AString strf(
"\
#%LUA%!{\
local val = alibrary.Model_emitContentFromPath(\"/root/data\"); \n\
val = val + 3; \n\
val = val .. \" and more\"; \n\
alibrary.Model_addElementText(\"/newElement/item\", val); \n\
alibrary.print(alibrary.Model_emitXml(0)); \n\
}!%LUA%##%CODE%!{print(/root)}!%CODE%#\
");

  AXmlDocument *pdoc = new AXmlDocument(ASW("root",4));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper no swiping! 0"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper No swiping! 1"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("Swiper NO Swiping! 2"));
  pdoc->useRoot().addElement("data").addData(ASWNL("5"));

  ARope rope;
  ATemplate t(pdoc, &rope);
  ATEMPLATE_USE_NODE(t, ATemplateNode_Lua);
  ATEMPLATE_USE_NODE(t, ATemplateNode_Code);

  {
    AXmlElement *pElement = new AXmlElement(ASW("root",4));
    pElement->addElement("sub0").addAttribute("attr", "zero");
    t.useObjects().insert("rootelem", new AObject<AXmlElement>(*pElement));
  }

  t.useObjects().insert("simplestring", new AObject<AString>("Simple string I added"));
  
  t.fromAFile(strf);
//  t.debugDump(); 
  
  try
  {
    t.process();
  }
  catch(AException& ex)
  {
    std::cout << ex << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }
  std::cout << t.useOutput() << std::endl;

  delete pdoc;
  return 1;
}