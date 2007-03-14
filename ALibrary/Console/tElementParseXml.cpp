#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AAttributes.hpp"
#include "ARope.hpp"
#include "AFile_AString.hpp"

int main()
{
  try
  {
    AXmlElement element;
//    AFile_AString strfile("<  boat  > ss minnow < anchor /> sink </boat>");
//    AFile_AString strfile("<boat>  <box size='3'> yarn </box><dumpster/> <barrel diameter='12'/></boat>");
    AFile_AString strfile("<boat>  <box size='3'> yarn </box>   <crate>vase</crate> <dumpster/> <barrel diameter='12'/></boat>");

    element.fromAFile(strfile);

    ARope rope;
    element.emitXml(rope);
    std::cout << rope << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }

  return 0;
}