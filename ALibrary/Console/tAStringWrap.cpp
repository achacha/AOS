#include "AString.hpp"

int main()
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

  return 0;
}