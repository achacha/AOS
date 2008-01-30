#include "AFile_AString.hpp"

void testFind()
{
  AFile_AString strfile;
  strfile.useAString().append("\
GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nCookie: foo=bar\r\nContent-Length: 11\r\n\r\na=1&b=2&c=3");
  
  size_t pos = 0;
  if (AConstant::npos != (pos = strfile.find(ASW("\r\n\r\n", 4))))
  {
    std::cout << "Found at " << pos << std::endl;
  }
  else
  {
    std::cerr << "Not found" << std::endl;
  }

  AString str;
  while (AConstant::npos != pos)
  {
    pos = strfile.readLine(str);
    std::cout << str << std::endl;
    str.clear();
  }
}

int main()
{
  testFind();
  
  return 0;
}
