#include "AString.hpp"
#include "AFile_IOStream.hpp"

int main()
{
  AFile_IOStream iosfile;
  AString str;
  while (AString::npos != iosfile.readLine(str))
  {
    std::cout << "'" << str << "'=" << str.getHash() << "\t" << str.getHash(0xff) << "\t" << str.getHash(0xf) << std::endl;
    str.clear();
  }
}