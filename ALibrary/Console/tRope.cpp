#include "ARope.hpp"
#include "AFile_AString.hpp"

void testRope()
{
  ARope rope("TextBlock0000", 8);
  rope.append("TextBlock0123456789");

  AString str;
  rope.emit(str);
  std::cout << str << std::endl;
  std::cout << rope << std::endl;

  ARope ropeCopy(rope);
  ropeCopy.emit(str);
  std::cout << str << std::endl;
  std::cout << ropeCopy << std::endl;
}

void testFileIO()
{
  AFile_AString strfile("Buffer09123456789Test0123456789");
  ARope rope;

  rope.read(strfile, AConstant::npos);
  AString str;
  rope.emit(str);

  std::cout << str << std::endl;

  strfile.clear();
  strfile.emit(rope);

  std::cout << strfile.useAString() << std::endl;
}

void testPeek()
{
  AFile_AString strfile;
  ARope rope(ASWNL("0123456789"), 4);

  rope.peek(strfile, 3);
  std::cout << strfile << std::endl;

  AString str;
  rope.peek(str, 7, 2);
  std::cout << str << std::endl;
}

int main ()
{
  //testRope();
  //testFileIO();
  testPeek();

  return 0;
}