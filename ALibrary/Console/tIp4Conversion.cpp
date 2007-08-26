#include "AString.hpp"
#include "ASocketLibrary.hpp"

int main()
{
  AString strIp("1.2.3.4");
  std::cout << "strIp=" << strIp << std::endl;
  u4 u4Ip = ASocketLibrary::convertStringToIp4(strIp);
  std::cout << "u4Ip=" << u4Ip << std::endl;

  strIp.clear();
  ASocketLibrary::convertIp4ToString(u4Ip, strIp);
  std::cout << "strIp=" << strIp << std::endl;

  return 0;
}