#include "AINIProfile.hpp"

int main()
{
  AINIProfile ini("aosconfig/AObjectServer.ini");
  ini.parse();
  ini.debugDump();

  return 0;
}
