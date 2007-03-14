#include "AChecksum.hpp"
#include "AString.hpp"
#include "ATextGenerator.hpp"

void testAdler32()
{
  AString data("0123456789");

  u4 adler = AChecksum::adler32(data);
  if (0x0aff020e != adler)
    std::cout << "Does not match!   " << std::hex << adler << std::endl;
  else
    std::cout << "Match!" << std::endl;
}

void testAdler32Cycle()
{
  u1 sss[129];
  AString str(256, 128);

  for (int i=0; i<10; ++i)
  {
    str.clear();
    ATextGenerator::generateRandomString(str, 16);
    memcpy(sss, str.data(), 128);

    std::cout << "adler32(" << str << ")=" << std::hex << AChecksum::adler32(str) << std::endl;
  }
}

int main()
{
  testAdler32();
  //testAdler32Cycle();

  return 0;
}
