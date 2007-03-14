#include <AString.hpp>

void testJustify()
{
  AString str;

  str.assign("2f8");
  str.justifyRight(4, '0');
  std::cout << str << std::endl;

  str.justifyRight(8, '0');
  std::cout << str << std::endl;

  str.justifyRight(3, '0');
  std::cout << str << std::endl;

  str.assign("2f8");
  str.justifyCenter(8, '0');
  std::cout << str << std::endl;

  str.justifyCenter(11, '0');
  std::cout << str << std::endl;

  str.assign("2f8");
  str.justifyCenter(11, '0');
  std::cout << str << std::endl;
}

void testConvert()
{
  double d = 13666.71727;
  std::cout << AString::fromDouble(d) << std::endl;
  std::cout << AString::fromDouble(d,1) << std::endl;
  std::cout << AString::fromDouble(d,3) << std::endl;
  std::cout << AString::fromDouble(d,7) << std::endl;

  std::cout << AString::fromDouble(0.00000012345,7) << std::endl;
}

int main()
{
  //testJustify();
  testConvert();

  return 0;
}