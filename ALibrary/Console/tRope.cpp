#include "ARope.hpp"
#include "AFile_AString.hpp"

void testRope()
{
  ARope rope("TextBlock0000", 8);
  rope.append("TextBlock0123456789");

  AString str;
  rope.emitString(str);
  std::cout << str << std::endl;
  std::cout << rope.toString() << std::endl;

  ARope ropeCopy(rope);
  ropeCopy.emitString(str);
  std::cout << str << std::endl;
  std::cout << ropeCopy.toString() << std::endl;
}

void testFileIO()
{
  AFile_AString strfile("Buffer09123456789Test0123456789");
  ARope rope;

  rope.fromAFile(strfile, AString::npos);
  AString str;
  rope.emitString(str);

  std::cout << str << std::endl;

  strfile.clear();
  strfile.writeRope(rope);

  std::cout << strfile.useAString() << std::endl;
}

int main ()
{
  //testRope();
  testFileIO();

  return 0;
}