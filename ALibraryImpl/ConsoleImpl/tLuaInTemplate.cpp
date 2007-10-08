#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "AString.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "ATemplateNode_Lua.hpp"

int main()
{
  AFile_AString strf(
"\
%%LUA%%{\
alibrary.print(alibrary.emit(\"simplestring\")); \n\
alibrary.print(alibrary.AsAXmlElement_emitXml(\"rootelem\")); \n\
alibrary.print(alibrary.AsAXmlElement_emitJson(\"rootelem\")); \n\
}%%LUA%%\
");

  AXmlDocument *pdoc = new AXmlDocument(ASW("root",4));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper no swiping! 0"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper No swiping! 1"));
  pdoc->useRoot().addElement("swiper").addData(ASWNL("Swiper NO Swiping! 2"));

  ARope rope;
  ATemplate t(pdoc, &rope);
  ATEMPLATE_USE_NODE(t, ATemplateNode_Lua);

  {
    AXmlElement *pElement = new AXmlElement(ASW("root",4));
    pElement->addElement("sub0").addAttribute("attr", "zero");
    t.useObjects().insert("rootelem", new AObject<AXmlElement>(*pElement));
  }

  t.useObjects().insert("simplestring", new AObject<AString>("Simple string I added"));
  
  t.fromAFile(strf);
  t.debugDump(); 
  t.process();
  std::cout << t.useOutput() << std::endl;

  return 1;
}