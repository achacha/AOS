#include "AString.hpp"
#include "ALuaEmbed.hpp"
#include "AException.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"

int main()
{
//  AFile_AString strf(
//"\
//alibrary.print(alibrary.emitXmlDocumentFromPath(\"/xml\", \"/root/swiper\")); \n\
//alibrary.print(alibrary.emit(\"/objects/simplestring\")); \n\
//");
  
  AFile_AString strf(
"\
alibrary.print(alibrary.emit(\"/objects/simplestring\")); \n\
alibrary.print(alibrary.emitXml(\"/xml/rootelem\")); \n\
alibrary.print(alibrary.emitJson(\"/xml/rootelem\")); \n\
");

  ALuaEmbed lua;
  {
    AXmlDocument *pdoc = new AXmlDocument(ASW("root",4));
    pdoc->useRoot().addElement("swiper").addData(ASWNL("This is my data 0"));
    pdoc->useRoot().addElement("swiper").addData(ASWNL("This is my data 1"));
    pdoc->useRoot().addElement("swiper").addData(ASWNL("This is my data 2"));
    lua.useObjects().insert("/xmldoc", pdoc);
  }

  {
    AXmlElement *pElement = new AXmlElement(ASW("root",4));
    pElement->addElement("sub0").addAttribute("attr", "zero");
    lua.useObjects().insert("/xml/rootelem", new AObject<AXmlElement>(*pElement));
  }

  lua.useObjects().insert("/objects/simplestring", new AObject<AString>("Simple string I added"));

  
  //    lua.useObjects().debugDump();

  ARope error;
  if (!lua.execute(strf, error))
  {
    std::cerr << "ERROR: " << error << std::endl;
  }
  std::cout << lua.useOutputBuffer() << std::endl;

  return 1;
}