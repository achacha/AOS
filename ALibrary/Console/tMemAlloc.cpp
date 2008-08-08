//#define _CRTDBG_MAP_ALLOC 1
//#include "crtdbg.h"
#include "AString.hpp"
#include "AEventVisitor.hpp"
#include "templateAutoPtr.hpp"
#include "AException.hpp"
#include "ATextGenerator.hpp"

void testNew()
{
  AString *p = new AString("foo",3);
  p->append("bar",3);
  delete p;
  p = NULL;
}

void testEventVisitor()
{
  AAutoPtr<AEventVisitor> p(new AEventVisitor(), true);
  p->startEvent(ASWNL("001"));
  p->startEvent(ASWNL("002"));
  p->startEvent(ASWNL("003"));
  p->startEvent(ASWNL("004"));
  p->addEvent(ASWNL("100"));
  p->startEvent(ASWNL("005"));
}

void testAString()
{
  AString str(3,4);
  str.assign("111",3);
  str.clear();
  str.assign("666666",6);
  str.clear();
  str.assign("22",2);
}

void testATextGenerator()
{
  AString str;
  //{
  //  u4 sizeNeeded = 25;
  //  std::cerr << "ATextGenerator::generateUniqueId size: " << sizeNeeded << std::endl;
  //  ATextGenerator::generateUniqueId(str, sizeNeeded);
  //  if (str.getSize() != sizeNeeded)
  //  {
  //    std::cerr << "ATextGenerator::generateUniqueId failed for size: " << sizeNeeded << std::endl;
  //    return;
  //  }
  //  str.clear();
  //}

  {
    u4 sizeNeeded = 128;
    std::cerr << "ATextGenerator::generateUniqueId size: " << sizeNeeded << std::endl;
    ATextGenerator::generateUniqueId(str, sizeNeeded);
    if (str.getSize() != sizeNeeded)
    {
      std::cerr << "ATextGenerator::generateUniqueId failed for size: " << sizeNeeded << std::endl;
      return;
    }
    str.clear();
  }
}

//void __cdecl Dumper(void *ptr, void *)
//{
//  int block = _CrtReportBlockType(ptr);
//  _RPT3(_CRT_WARN, "Dumper found block at %p: type %d, subtype %d\n", ptr, _BLOCK_TYPE(block), _BLOCK_SUBTYPE(block));
//}
//
//void __cdecl LeakDumper(void *ptr, size_t sz)
//{
//  int block = _CrtReportBlockType(ptr);
//  _RPT4(_CRT_WARN, "LeakDumper found block at %p: type %d, subtype %d, size %d\n", ptr, _BLOCK_TYPE(block), _BLOCK_SUBTYPE(block), sz);
//}

int main()
{
DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(true);

  try
  {
    //testNew();
    //testEventVisitor();
    //testAString();
    testATextGenerator();
  }
  catch(AException& e)
  {
    std::cerr << e << std::endl;
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception" << std::endl;
  }

DEBUG_MEMORY_LEAK_ANALYSIS_END();

  return 0;
}