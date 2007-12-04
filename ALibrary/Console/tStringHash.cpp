#include <iostream>
#include <AString.hpp>
#include <AFileIOStream.hpp>
#include <ATimer.hpp>
#include <ARandomNumberGenerator.hpp>

void testHashing()
{
  AString strX;
  ATimer timerX;

  timerX.start();

  for (int iX = 0x0; iX < 0x20; ++iX)
  {
    strX = ARandomNumberGenerator::getRandomString(0x10);
    
    cerr << (const char*)strX << " : " << strX.getHash(0xD) << endl;
  }

  timerX.stop();

  cerr << "Timer=" << timerX.getInterval() << "ms" << endl;
}

int main()
{
  testHashing();
  return 0x0;
}