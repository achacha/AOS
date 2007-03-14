#include "AString.hpp"
#include <iostream>

void testGetUntil()
{
  AString strT, str("This is my test");

  while (str.getUntil(strT) != AString::npos)
  {
    std::cout << "'" << strT.c_str() << "'" << std::endl;
  }

}

void testPeekUntil()
{
  AString strT, str("This is my test");

  u4 x = 0;
  while ((x < str.getSize()) && (x = str.peekUntil(strT, x)))
  {
    ++x;

    std::cout << "'" << strT.c_str() << "'" << std::endl;
  }
}

int main()
{
//  testGetUntil();
  testPeekUntil();

  return 0x0;
}