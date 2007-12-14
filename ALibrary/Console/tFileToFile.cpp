#include "AFile_AString.hpp"

void testReadWrite()
{
  {
    AFile_AString source("0123456789");
    AFile_AString target;

    target.write(source, 4);
    target.write(source, 2);
    target.write(source, 5);
    std::cout << target << std::endl;
  }
}



int main()
{
  testReadWrite();

  return 0;
}