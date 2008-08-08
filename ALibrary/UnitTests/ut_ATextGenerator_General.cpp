/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <ATextGenerator.hpp>

int ut_ATextGenerator_General()
{
  std::cerr << "ut_ATextGenerator_General" << std::endl;

  int iRet = 0x0;

  AString str;
  bool errorOccured = false;
  for (int iX=0; iX < 666; ++iX)
  {
    u4 sizeNeeded = ARandomNumberGenerator::get().nextRange(300, 16);
    std::cerr << "ATextGenerator::generateUniqueId size: " << sizeNeeded << std::endl;
    ATextGenerator::generateUniqueId(str, sizeNeeded);
    if (str.getSize() != sizeNeeded)
    {
      std::cerr << "ATextGenerator::generateUniqueId failed for size: " << sizeNeeded << std::endl;
      iRet++;
      errorOccured = true;
      break;
    }
    str.clear();
  }
  if (!errorOccured)
    std::cerr << "." << std::flush;

  return iRet;
}

