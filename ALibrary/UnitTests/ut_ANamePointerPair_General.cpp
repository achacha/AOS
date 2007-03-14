#include "pchUnitTests.hpp"

#include <ANamePointerPair.hpp>

int ut_ANamePointerPair_General()
{
  std::cerr << "ut_ANamePointerPair_General" << std::endl;

  int iRet = 0x0;
  
  ANamePointerPair nvPair("name");
  AString str;
  nvPair.emit(str);
  if (str.compare("name=0x0"))
  {
    iRet++;
    std::cerr << "ANamePointerPair::emit to AString failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if (nvPair.isName("Name"))
  {
    iRet++;
    std::cerr << "isName failed: " << nvPair.getName().c_str() << "==Name" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if (!nvPair.isNameNoCase("Name"))
  {
    iRet++;
    std::cerr << "isNameNoCase failed: " << nvPair.getName().c_str() << "!=Name" << std::endl;
  } else std::cerr << "." << std::flush;

  return iRet;
}
