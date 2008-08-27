#include <AString.hpp>

int main()
{
  AString str0(""), str1("012");

  if (str0 > str1)
    std::cerr << str0 << ">" << str1 << std::endl;
  else
    std::cerr << str0 << "<=" << str1 << std::endl;

  return 0x0;
}