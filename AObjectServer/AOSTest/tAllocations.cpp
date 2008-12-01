
#include "AOS.hpp"


#ifndef NDEBUG
#include "MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#endif


int main()
{
  AEventVisitor ev;
  ev.startEvent(ASWNL("This is a test"));

  return 0;
}