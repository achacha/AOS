//#define _CRTDBG_MAP_ALLOC 1
//#include "crtdbg.h"
#include "AString.hpp"
#include "AEventVisitor.hpp"
#include "templateAutoPtr.hpp"
#include "AException.hpp"
#include "ATextGenerator.hpp"
#include "ARope.hpp"
#include "ARopeDeque.hpp"
#include "AThread.hpp"
#include "AFile_IOStream.hpp"

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
  {
    u4 sizeNeeded = 666;
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

void testARope()
{
  ARopeDeque rope(AConstant::ASTRING_EMPTY, 1536);
  AString input;
  AFile_IOStream iof;

  iof.append("Start [Press Enter]?");
  iof.readLine(input);
  for (int i = 0; i < 1000; ++i)
  {
    iof.append("Allocating...");
    for (int j = 0; j < 5000; ++j)
    {
      ATextGenerator::generateRandomAlphanum(rope, 2048);
    }
    iof.append("allocated [Press Enter]?");
    iof.readLine(input);
    rope.clear(false);
    iof.append("Cleared [Press Enter]?");
    rope.clear(true);
    iof.append("Fully cleared [Press Enter]?");
    iof.readLine(input);
  }
}

int main()
{
DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(true);

  try
  {
    testNew();
    //testEventVisitor();
    //testAString();
    //testATextGenerator();
    //testARope();
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