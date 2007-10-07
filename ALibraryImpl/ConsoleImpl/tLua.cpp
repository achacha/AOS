#include "AString.hpp"
#include "ALuaEmbed.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "templateAObject.hpp"

int main()
{
  AFile_AString strf(
"\
alibrary.print(alibrary.emit(\"simplestring\")); \n\
alibrary.print(alibrary.AsAXmlElement_emitXml(\"rootelem\")); \n\
alibrary.print(alibrary.AsAXmlElement_emitJson(\"rootelem\")); \n\
");

  ALuaEmbed lua;
  {
    AXmlDocument *pdoc = new AXmlDocument(ASW("root",4));
    pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper no swiping! 0"));
    pdoc->useRoot().addElement("swiper").addData(ASWNL("swiper No swiping! 1"));
    pdoc->useRoot().addElement("swiper").addData(ASWNL("Swiper NO Swiping! 2"));
    lua.useObjectHolder().insert("xmldoc", pdoc);
  }

  {
    AXmlElement *pElement = new AXmlElement(ASW("root",4));
    pElement->addElement("sub0").addAttribute("attr", "zero");
    lua.useObjectHolder().insert("rootelem", new AObject<AXmlElement>(*pElement));
  }

  lua.useObjectHolder().insert("simplestring", new AObject<AString>("Simple string I added"));

  
  //    lua.useObjectHolder().debugDump();

  ARope error;
  if (!lua.execute(strf))
  {
    std::cerr << "ERROR: " << lua.useOutputBuffer() << std::endl;
  }
  std::cout << lua.useOutputBuffer() << std::endl;

  return 1;
}