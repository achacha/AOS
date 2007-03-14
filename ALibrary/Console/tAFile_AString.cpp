#include "AFile_AString.hpp"

void testBasicRead()
{
  AString str;
  AFile_AString strfile("Testing");

  strfile.read(str, 4);
  std::cout << str << std::endl;
  strfile.read(str, 2);
  std::cout << str << std::endl;
  strfile.read(str, 3);
  std::cout << str << std::endl;
}

void testBasicWrite()
{
  AString str;
  AFile_AString strfile;

  strfile.write(AString("Test\n"));
  strfile.setWritePos(4);
  strfile.writeLine("Line 000");
  strfile.setReadPos(4);
  strfile.readUntil(str);
  std::cout << str << std::endl;
  strfile.readUntil(str);
  std::cout << str << std::endl;
}


void testLineRead()
{
  AString str;
  AFile_AString strfile("This is a test\nLine2\n");

  strfile.readLine(str);
  std::cout << str << std::endl;

  strfile.write(AString("Line3"));

  strfile.readLine(str);
  std::cout << str << std::endl;

  strfile.readLine(str);
  std::cout << str << std::endl;
}

void testOneCharRead()
{
  AFile_AString strfile("On and on it's south of heaven.");
  
  char c;

  while (strfile.read(c) == 1)
  {
    std::cout << c << std::flush;
  }
  std::cout << std::endl;
}

void testPeekUntil()
{
//TODO:

}


int main()
{
  //testBasicRead();
  //testBasicWrite();
  //testLineRead();
  //testOneCharRead();
  testPeekUntil();

  return 0;
}
