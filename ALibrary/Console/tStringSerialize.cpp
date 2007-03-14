#include <iostream.h>
#include <AString.hpp>
#include <AFileIOStream.hpp>

int main()
{
  AString str("Test");
  AFileIOStream file;

  file << str;

  file << endl;
  return 0x0;
}