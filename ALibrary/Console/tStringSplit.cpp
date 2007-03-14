#include <AString.hpp>

void testSplit(const AString& str)
{
  LIST_AString names;
  str.split(names, '/');

  std::cout << "Input: '" << str << "'" << std::endl;
  LIST_AString::iterator it = names.begin();
  while(it != names.end())
  {
    std::cout << *it << std::endl;
    ++it;
  }
  std::cout << std::endl;
}

int main()
{
  testSplit(AString("/path0/path1/path2"));
  testSplit(AString("path0/path1/path2/"));
  testSplit(AString("/path0/path1/path2/"));
  testSplit(AString("//path0///path1///path2////"));
  testSplit(AString("//"));
  testSplit(AString("path0"));
  testSplit(AString(""));

  return 0x0;
}