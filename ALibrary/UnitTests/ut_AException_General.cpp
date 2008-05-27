/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"

#include <AException.hpp>

int ut_AException_General()
{
  std::cerr << "ut_AException_General" << std::endl;

  int iRet = 0x0;
  bool boolCaught = false;
  try
  {
    ATHROW(NULL, AException::ProgrammingError);
  }
  catch(AException& eX)
  {
    AException eCopy(eX);
    if (AString(eX.what()) != eCopy.what())
    {
      ++iRet;
      std::cerr << "AException copy constructor did not work" << std::endl;
    } else std::cerr << "." << std::flush;
    boolCaught = true;
  }
  catch(...)
  {
    std::cerr << "Error: Unhandled exception caught." << std::endl;
  }
  if (!boolCaught)
  {
    ++iRet;
    std::cerr << "Expected AException-type was not thrown!" << std::endl;
  }

  boolCaught = false;
  try
  {
    ATHROW(NULL, AException::ProgrammingError);
  }
  catch(AException& eX)
  {
    AException eCopy(eX);
    if (AString(eX.what()) != eCopy.what())
    {
      ++iRet;
      std::cerr << "AStringException copy constructor did not work" << std::endl;
    } else std::cerr << "." << std::flush;

    boolCaught = true;
  }
  catch(...)
  {
    std::cerr << "Error: Unhandled exception caught." << std::endl;
  }
  if (!boolCaught)
  {
    ++iRet;
    std::cerr << "Expected AException-type was not thrown!" << std::endl;
  }

  boolCaught = false;
  try
  {
    ATHROW(NULL, AException::IndexOutOfBounds);
  }
  catch(AException& eX)
  {
    AException eCopy(eX);
    if (AString(eX.what()) != eCopy.what())
    {
      ++iRet;
      std::cerr << "AStringException copy constructor did not work" << std::endl;
    } else std::cerr << "." << std::flush;

    boolCaught = true;
  }
  catch(...)
  {
    std::cerr << "Error: Unhandled exception caught." << std::endl;
  }
  if (!boolCaught)
  {
    ++iRet;
    std::cerr << "Expected AException-type was not thrown!" << std::endl;
  }
  return 0x0;
}
