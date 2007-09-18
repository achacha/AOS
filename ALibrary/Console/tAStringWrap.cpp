#include "AString.hpp"

void testWrap0()
{
  const AString& temp = AString::wrap("This,is,a,test"); 
  VECTOR_AString vec;

  temp.split(vec, ',');

  std::cout << temp << std::endl;
  VECTOR_AString::iterator it = vec.begin();
  while (it != vec.end())
  {
    std::cout << *it << std::endl;
    ++it;
  }
}

void testWrap1()
{
  const char *pcc = "test";
  const AString& foo = AString::wrap(pcc);
  AString *pstr = new AString(foo);

  std::cout << "?" << std::endl;
}

int main()
{
  //testWrap0();
  testWrap1();

  return 0;
}