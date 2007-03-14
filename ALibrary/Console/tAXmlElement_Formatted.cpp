
#include "AXmlElement.hpp"
#include "ARope.hpp"
#include "AFile_AString.hpp"

int main(int argc, char* argv[])
{
  try
  {
    //a_XML dump
    AFile_AString stringfile("<root><c_0><gc_00/><gc_01>5</gc_01></c_0><c_1><gc_10/><gc_11/></c_1></root>");
    AXmlElement element;
    element.fromAFile(stringfile);
    ARope rope;
    element.emitXml(rope, 0);
  //  element.debugDump();
    std::cout << rope.toString() << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  return 0;
}
