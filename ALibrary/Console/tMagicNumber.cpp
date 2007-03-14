#include <iostream.h>
#include <AMagicNumber.hpp>
#include <AFileIOStream.hpp>
#include <ATime.hpp>

int main()
{
  AFileIOStream file(&cerr);

  file << AMagicNumber("AClass") << endl;
  file << AMagicNumber("AClsas") << endl;
  file << AMagicNumber("ACalss") << endl;
  file << AMagicNumber("ssalCA") << endl;
  file << AMagicNumber("aClass") << endl;
  file << AMagicNumber("AClass0") << endl;
  file << AMagicNumber("AClas") << endl;
  file << AMagicNumber("A") << endl;
  file << AMagicNumber() << endl;

  u4 u4End = 0x0,
     u4Start = ATime::getTickCount();

  //a_Performance test
  for (int iT = 0x0; iT < 32767; ++iT)
  {
    AMagicNumber("AClass01234567890");
    AMagicNumber("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    AMagicNumber("a0b1c2d3e4f5");
    AMagicNumber("0a1b2c3d4e5f");
    AMagicNumber("01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    AMagicNumber("A");
    AMagicNumber();
  }
  u4End = ATime::getTickCount();
  
  cout << "Time=" << u4End - u4Start << " ms" << endl;

  return 0x0;
}