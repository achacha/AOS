#include "AString.hpp"
#include "ARope.hpp"
#include "ATimer.hpp"

#define BLOCK 2048

int main()
{
  ARope rope(BLOCK);
  AString str(BLOCK, BLOCK);
  ATimer timer;
  int i;
  const AString foo("AChachanashvili");

  timer.start();
  for(i=0; i<50000; ++i)
  {
    str.append(foo);
  }
  timer.stop();
  std::cout << "AString=" << timer.getInterval() << std::endl;
  timer.reset();

  timer.start();
  for(i=0; i<50000; ++i)
  {
    rope.append(foo);
  }
  timer.stop();
  std::cout << "ARope=" << timer.getInterval() << std::endl;

  return 0;
}
