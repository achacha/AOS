#include "AOS.hpp"
#include "AOSSessionData.hpp"

int main()
{
  AFile_AString file;
  AOSSessionData data0(ASWNL("ABC123"));
  data0.toAFile(file);
  AOSSessionData data1(file);

  file.debugDump();
  std::cout << data0 << std::endl;
  std::cout << data1 << std::endl;

  return 0;
}