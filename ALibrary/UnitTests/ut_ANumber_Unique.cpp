/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include <ANumber.hpp>
#include <AException.hpp>

void _testUniqueCases(int& iRet)
{
  bool boolCaught = false;
  ANumber nInf;
  nInf.setInfinite();

  if (!((ANumber(13) / ANumber(0)).isInfinite()))
  {
    iRet++;
    std::cerr << "ANumber::operator / failed: 13/0 != Infinity" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber(0) / ANumber(13)) != ANumber("0"))
  {
    iRet++;
    std::cerr << "ANumber::operator / failed: 0/13 != 0" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber(13) / nInf) != ANumber("0"))
  {
    iRet++;
    std::cerr << "ANumber::operator / failed: 13/Inf != 0" << std::endl;
  } else std::cerr << "." << std::flush;


  boolCaught = false;
  try
  {
    std::cerr << "0 / Infinity = " <<
		(ANumber(0) / nInf).toAString() 
		<< std::endl
		;
  }
  catch(AException& /*eX)*/)
  {
    boolCaught = true;
//    std::cerr << eX.what() << std::endl;
  }
  catch(...)
  {
    boolCaught = true;
    iRet++;
    std::cerr << "0/Inf: Exception not handled.";
  }
  if (!boolCaught)
  {
    iRet++;
    std::cerr << "0/Inf: Exception not caught.";
  }

  boolCaught = false;
  try
  {
//    std::cerr << "Infinity / 0 = " << 
		(nInf / ANumber(0)).toAString()
//		<< std::endl
		;
  }
  catch(AException& /*eX*/)
  {
    boolCaught = true;
//    std::cerr << eX.what() << std::endl;
  }
  catch(...)
  {
    boolCaught = true;
    iRet++;
    std::cerr << "Inf/0: Exception not handled.";
  }
  if (!boolCaught)
  {
    iRet++;
    std::cerr << "Inf/0: Exception not caught.";
  }

  boolCaught = false;
  try
  {
//    std::cerr << "0 / 0 = " <<
		(ANumber(0) / ANumber(0)).toAString()
//		<< std::endl
		;
  }     
  catch(AException& /*eX*/)
  {
    boolCaught = true;
//    std::cerr << eX.what() << std::endl;
  }
  catch(...)
  {
    boolCaught = true;
    iRet++;
    std::cerr << "0/0: Exception not handled.";
  }
  if (!boolCaught)
  {
    iRet++;
    std::cerr << "0/0: Exception not caught.";
  }

  boolCaught = false;
  try
  {
//    std::cerr << "Infinity * 0 = " << 
		(nInf * ANumber(0)).toAString()
//		<< std::endl
		;
  }
  catch(AException& /*eX*/)
  {
    boolCaught = true;
//    std::cerr << eX.what() << std::endl;
  }
  catch(...)
  {
    boolCaught = true;
    iRet++;
    std::cerr << "Inf*0: Exception not handled.";
  }
  if (!boolCaught)
  {
    iRet++;
    std::cerr << "Inf*0: Exception not caught.";
  }

  boolCaught = false;
  try
  {
//    std::cerr << "13 / Infinity = " << 
		(ANumber(13) / nInf).toAString()
//		<< std::endl
		;
  }
  catch(AException &eX)
  {
    boolCaught = true;
    std::cerr << eX.what() << std::endl;
  }
  catch(...)
  {
    boolCaught = true;
    iRet++;
    std::cerr << "13/Inf: Exception not handled.";
  }
  if (boolCaught)
  {
    iRet++;
    std::cerr << "13/Inf: Exception caught.";
  }

}

int ut_ANumber_Unique(void)
{
  int iRet = 0x0;

  std::cerr << "ut_ANumber_Unique" << std::endl;

  _testUniqueCases(iRet);

  return iRet;
}

