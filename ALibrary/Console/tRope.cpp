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

  size_t r;
  r = rope.peek(strfile, 3);
  std::cout << r << ":" << strfile << std::endl;

  AString str;
  r = rope.peek(str);
  std::cout << r << ":" << str << std::endl;

  str.clear();
  r = rope.peek(str, 7, 2);
  std::cout << r << ":" << str << std::endl;

  str.clear();
  r = rope.peek(str, 7, 0);
  std::cout << r << ":" << str << std::endl;

  str.clear();
  r = rope.peek(str, 1, 50);
  std::cout << r << ":" << str << std::endl;

  str.clear();
  r = rope.peek(str, 10, 1);
  std::cout << r << ":" << str << std::endl;
}

int main ()
{
  try
  {
    //testRope();
    //testFileIO();
    testPeek();
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }

  return 0;
}