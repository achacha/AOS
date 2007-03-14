#include "AString.hpp"
#include "ATextGenerator.hpp"
#include "ATextConverter.hpp"

void testRandomWord()
{
  AString str;
  for (int i=0; i<10; ++i)
  {
    ATextGenerator::generateRandomWord(str, 6);
    std::cout << str << std::endl;
    str.clear();
  }
}

void testUniqueId()
{
  AString str;
  ATextGenerator::generateUniqueId(str, 17);
  std::cout << str.getSize() << ":" << str << std::endl;
}

int main()
{
  //testRandomWord();
  testUniqueId();
  return 0;
}