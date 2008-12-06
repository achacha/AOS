/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <ANumber.hpp>
#include <AException.hpp>

void _utTestSetNumber(int& iRet)
{
  ANumber nX("6661.3E-1");      //a_My 2 lucky numberz....  666.13
  if ( nX.toAString().compare("666.13") )
  {
    iRet++;
    std::cerr << "ANumber::setNumber failed:" << nX.toAString().c_str() << "!=666.13" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = ".5";
  if ( nX.toAString().compare("0.5") )
  {
    iRet++;
    std::cerr << "ANumber::setNumber failed:" << nX.toAString().c_str() << "!=0.5" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.setNumber("-.5");
  if ( nX.toAString().compare("-0.5") )
  {
    iRet++;
    std::cerr << "ANumber::setNumber failed:" << nX.toAString().c_str() << "!=-0.5" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.setNumber("-13");
  if ( nX.toAString().compare("-13") )
  {
    iRet++;
    std::cerr << "ANumber::setNumber failed:" << nX.toAString().c_str() << "!=-13" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = "0";
  if ( nX.toAString().compare("0") )
  {
    iRet++;
    std::cerr << "ANumber::operator= (const AString&) failed:" << nX.toAString().c_str() << "!=0" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = "-1";
  if ( nX.toAString().compare("-1") )
  {
    iRet++;
    std::cerr << "ANumber::operator= (const AString&) failed:" << nX.toAString().c_str() << "!=-1" << std::endl;
  } else std::cerr << "." << std::flush;

  bool boolCaught = false;
  try
  {
    nX.setNumber(AString(".5.3A"));      //a_This will throw exception
  }
  catch(AException&)
  {
    boolCaught = true;
  }
  catch(...)
  {
  }
  if (!boolCaught)
  {
    iRet++;
    std::cerr << "ANumber::setNumber exception not thrown!" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = "0.13";
  if ( nX.toAString().compare("0.13") )
  {
    iRet++;
    std::cerr << "ANumber::operator=(const AString&) failed:" << nX.toAString().c_str() << "!=-1" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = int(13);
  if ( nX.toAString().compare("13") )
  {
    iRet++;
    std::cerr << "ANumber::operator= (int) failed:" << nX.toAString().c_str() << "!=-1" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = long(666L);
  if ( nX.toAString().compare("666") )
  {
    iRet++;
    std::cerr << "ANumber::operator= (long) failed:" << nX.toAString().c_str() << "!=-1" << std::endl;
  } else std::cerr << "." << std::flush;
}


//a_Addition and subtraction are really sum types (philosophical arguements welcome!)
void _testUnaryLinearOperators(int& iRet)
{
  ANumber nX;
  nX = "10";
  if ( nX.toAString().compare("10") )
  {
    iRet++;
    std::cerr << "ANumber::operator= failed:" << nX.toAString().c_str() << "!=10" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_operator +=
  nX += ANumber("5");
  if ( nX.toAString().compare("15") || nX != ANumber("15.0"))
  {
    iRet++;
    std::cerr << "ANumber::operator+= failed:" << nX.toAString().c_str() << "!=15" << std::endl;
  } else std::cerr << "." << std::flush;

  nX += ANumber("-2");
  if ( nX.toAString().compare("13") || nX != ANumber((int)13))
  {
    iRet++;
    std::cerr << "ANumber::operator+= failed:" << nX.toAString().c_str() << "!=13" << std::endl;
  } else std::cerr << "." << std::flush;

  nX += ANumber("-15");
  if ( nX.toAString().compare("-2") || nX != ANumber((long)-2))
  {
    iRet++;
    std::cerr << "ANumber::operator+= failed:" << nX.toAString().c_str() << "!=-2" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_operator += again with fractions
  nX = 0;
  if ( !nX.isZero() )
  {
    iRet++;
    std::cerr << "ANumber::operator= failed or isZero failed:" << nX.toAString().c_str() << "!=0" << std::endl;
  } else std::cerr << "." << std::flush;
  nX += ANumber("-.5");
  if ( nX != ANumber("-.5"))
  {
    iRet++;
    std::cerr << "ANumber::operator+= failed:" << nX.toAString().c_str() << "!=-.5" << std::endl;
  } else std::cerr << "." << std::flush;
  nX += ANumber("-2");
  if ( nX != ANumber("-2.5"))
  {
    iRet++;
    std::cerr << "ANumber::operator+= failed:" << nX.toAString().c_str() << "!=-2.5" << std::endl;
  } else std::cerr << "." << std::flush;
  nX += ANumber("+1.6");
  if ( nX != ANumber("-0.9"))
  {
    iRet++;
    std::cerr << "ANumber::operator+= failed:" << nX.toAString().c_str() << "!=-0.9" << std::endl;
  } else std::cerr << "." << std::flush;
  nX += ANumber("+5.1");
  if ( nX != ANumber("4.2"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=4.2" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_operator -=
  nX = 0;
  nX -= ANumber("-0.7");
  if ( nX != ANumber("0.7"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=0.7" << std::endl;
  } else std::cerr << "." << std::flush;
  nX -= ANumber("-3");
  if ( nX != ANumber("3.7"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=3.7" << std::endl;
  } else std::cerr << "." << std::flush;
  nX -= ANumber("1.8");
  if ( nX != ANumber("1.9"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=1.9" << std::endl;
  } else std::cerr << "." << std::flush;
  nX -= ANumber("3");
  if ( nX != ANumber("-1.1"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=-1.1" << std::endl;
  } else std::cerr << "." << std::flush;
  nX -= ANumber("-1");
  if ( nX != ANumber("-0.1"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=-0.1" << std::endl;
  } else std::cerr << "." << std::flush;
  nX -= ANumber("1");
  if ( nX != ANumber("-1.1"))
  {
    iRet++;
    std::cerr << "ANumber::opertor+= failed:" << nX.toAString().c_str() << "!=-1.1" << std::endl;
  } else std::cerr << "." << std::flush;
}

//a_Multiplication and division
void _testUnaryMultiplicativeOperators(int& iRet)
{
  ANumber nX;
  nX = "-0.5";
  nX *= ANumber("5");
  if ( nX != ANumber("-2.5"))
  {
    iRet++;
    std::cerr << "ANumber::opertor*= failed:" << nX.toAString().c_str() << "!=-2.5" << std::endl;
  } else std::cerr << "." << std::flush;

  nX *= ANumber("-0.5");
  if ( nX != ANumber("1.25"))
  {
    iRet++;
    std::cerr << "ANumber::opertor*= failed:" << nX.toAString().c_str() << "!=1.25" << std::endl;
  } else std::cerr << "." << std::flush;

  nX *= ANumber("-1");
  if ( nX != ANumber("-1.25"))
  {
    iRet++;
    std::cerr << "ANumber::opertor*= failed:" << nX.toAString().c_str() << "!=-1.25" << std::endl;
  } else std::cerr << "." << std::flush;

  nX *= ANumber("2.5");
  if ( nX != ANumber("-3.125"))
  {
    iRet++;
    std::cerr << "ANumber::opertor*= failed:" << nX.toAString().c_str() << "!=-3.125" << std::endl;
  } else std::cerr << "." << std::flush;

  nX /= ANumber(5);
  if ( nX != ANumber("-0.625"))
  {
    iRet++;
    std::cerr << "ANumber::opertor/= failed:" << nX.toAString().c_str() << "!=-0.625" << std::endl;
  } else std::cerr << "." << std::flush;

  nX /= ANumber("-1.0");
  if ( nX != ANumber("0.625"))
  {
    iRet++;
    std::cerr << "ANumber::opertor/= failed:" << nX.toAString().c_str() << "!=0.625" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testDivision(int& iRet)
{
  if ((ANumber(256) / ANumber(24)).toAString().compare("10.66666666666666666667"))
  {
    iRet++;
    std::cerr << "ANumber: 256/24 != 10.66666666666666666667" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if ((ANumber(240) / ANumber(24)).toAString().compare("10"))
  {
    iRet++;
    std::cerr << "ANumber: 240/24 != 10" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber(2401) / ANumber(24)).toAString().compare("100.04166666666666666667"))
  {
    iRet++;
    std::cerr << "ANumber: 2401/24 != 100.04166666666666666667" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber("-2.56") / ANumber(24)).toAString().compare("-0.10666666666666666667"))
  {
    iRet++;
    std::cerr << "ANumber: -2.56/24 != -0.10666666666666666667" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber(256) / ANumber("-0.24")).toAString().compare("-1066.66666666666666666667"))
  {
    iRet++;
    std::cerr << "ANumber: 256/-0.24 != -1066.66666666666666666667" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber(1024) / ANumber(120)).toAString().compare("8.53333333333333333333"))
  {
    iRet++;
    std::cerr << "ANumber: 1024/120 != 8.53333333333333333333" << std::endl;
  } else std::cerr << "." << std::flush;

  if ((ANumber("43046721") / ANumber("40320")).toAString().compare("1067.62700892857142857143"))
  {
    iRet++;
    std::cerr << "ANumber: 43046721 / 40320 != 1067.62700892857142857143" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testUnaryNonLinearOperators(int& iRet)
{
  ANumber nX;

  //a_Shifts
  nX = "0.0013";
  nX.shiftDecimalRight(5);
  if ( nX != ANumber("130"))
  {
    iRet++;
    std::cerr << "ANumber::shiftDecimalRight failed:" << nX.toAString().c_str() << "!=130" << std::endl;
  } else std::cerr << "." << std::flush;


  nX.shiftDecimalLeft(2);
  if ( nX != ANumber("1.3"))
  {
    iRet++;
    std::cerr << "ANumber::shiftDecimalLeft failed:" << nX.toAString().c_str() << "!=1.3" << std::endl;
  } else std::cerr << "." << std::flush;


  nX.shiftDecimalLeft(-4);
  if ( nX != ANumber("13000"))
  {
    iRet++;
    std::cerr << "ANumber::shiftDecimalLeft failed:" << nX.toAString().c_str() << "!=13000" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.shiftDecimalRight(-3);
  if ( nX != ANumber("13"))
  {
    iRet++;
    std::cerr << "ANumber::shiftDecimalRight failed:" << nX.toAString().c_str() << "!=13" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.shiftDecimalLeft(0);
  if ( nX != ANumber("13"))
  {
    iRet++;
    std::cerr << "ANumber::shiftDecimalLeft failed:" << nX.toAString().c_str() << "!=13" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Exponents
  nX = "-0.5";
  nX.power(ANumber(2));
  if ( nX != ANumber("0.25"))
  {
    iRet++;
    std::cerr << "ANumber::power failed:" << nX.toAString().c_str() << "!=0.25" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.power(ANumber(2));
  if ( nX != ANumber("0.0625"))
  {
    iRet++;
    std::cerr << "ANumber::power failed:" << nX.toAString().c_str() << "!=0.0625" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.power(ANumber(2));
  if ( nX != ANumber("0.00390625"))
  {
    iRet++;
    std::cerr << "ANumber::power failed:" << nX.toAString().c_str() << "!=0.00390625" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.setPrecision(32);
  nX.power(ANumber(2));
  if ( nX != ANumber("0.0000152587890625"))
  {
    iRet++;
    std::cerr << "ANumber::power failed:" << nX.toAString().c_str() << "!=0.0000152587890625" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = "2";
  nX.power(ANumber(16));
  if ( nX != ANumber("65536"))
  {
    iRet++;
    std::cerr << "ANumber::power failed:" << nX.toAString().c_str() << "!=65536" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = "-2";
  nX.power(ANumber(3));
  if ( nX != ANumber("-8"))
  {
    iRet++;
    std::cerr << "ANumber::power failed:" << nX.toAString().c_str() << "!=-8" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testBinaryOperators(int& iRet)
{
  ANumber nX;

  nX = ANumber(5) + ANumber("13.13") - ANumber("0.13");
  if (nX != ANumber("18"))
  {
    iRet++;
    std::cerr << "ANumber::binary operators failed:" << nX.toAString().c_str() << "!=-18" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = (-(ANumber("-3").power(ANumber(3))) - ANumber("7.0")) / ANumber("5.00000");
  if (nX != ANumber("4"))
  {
    iRet++;
    std::cerr << "ANumber::binary operators failed:" << nX.toAString().c_str() << "!=-18" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testPrecision(int& iRet)
{
  ANumber nX(5);

  nX.forcePrecision(6);
  if (nX.toAString().compare("5.000000"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(6) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.setPrecision(3);
  if (nX.toAString().compare("5.000"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(3) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.forcePrecision(1);
  if (nX.toAString().compare("5.0"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(1) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.setPrecision(0);
  if (nX.toAString().compare("5"))
  {
    iRet++;
    std::cerr << "ANumber::setPrecision(0) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.setPrecision(2);
  if (nX.toAString().compare("5"))
  {
    iRet++;
    std::cerr << "ANumber::setPrecision(2) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.forcePrecision(3);
  if (nX.toAString().compare("5.000"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(3) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Rounding
  nX.setPrecision(10);
  nX = ANumber(2) / ANumber(3);
  nX.setPrecision(6);
  if (nX.toAString().compare("0.666667"))
  {
    iRet++;
    std::cerr << "ANumber::setPrecision(6) failed" << std::endl;
  } else std::cerr << "." << std::flush;
  
  nX.forcePrecision(4);
  if (nX.toAString().compare("0.6667"))
  {
    iRet++;
    std::cerr << "ANumber::setPrecision(4) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  nX.forcePrecision(6);
  if (nX.toAString().compare("0.666700"))
  {
    iRet++;
    std::cerr << "ANumber::setPrecision(6) failed (up-set)" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testTests(int& iRet)
{
  ANumber nX(0);
  nX.forcePrecision(10);
  if (!nX.isZero())
  {
    iRet++;
    std::cerr << "ANumber::isZero: " << nX.toAString() << " is not zero" << std::endl;
  } else std::cerr << "." << std::flush;
  nX = "0.000000001";
  if (nX.isZero())
  {
    iRet++;
    std::cerr << "ANumber::isZero: " << nX.toAString() << " is zero" << std::endl;
  } else std::cerr << "." << std::flush;

  nX /= ANumber(0);
  if (!nX.isInfinite())
  {
    iRet++;
    std::cerr << "ANumber::isInfinite: " << nX.toAString() << " is not infinite" << std::endl;
  } else std::cerr << "." << std::flush;

  nX = 3;
  if (!nX.isWhole())
  {
    iRet++;
    std::cerr << "ANumber::isWhole: " << nX.toAString() << " is not whole" << std::endl;
  } else std::cerr << "." << std::flush;
  nX /= ANumber(2);
  if (nX.isWhole())
  {
    iRet++;
    std::cerr << "ANumber::isWhole: " << nX.toAString() << " is whole" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testComparison(int& iRet)
{
  if (!(ANumber(1) > ANumber(0)))
  {
    iRet++;
    std::cerr << "ANumber::operator > failed: ! 1>0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(1) < ANumber(0))
  {
    iRet++;
    std::cerr << "ANumber::operator < failed: 1<0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(1) == ANumber(0))
  {
    iRet++;
    std::cerr << "ANumber::operator == failed: 1==0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(-1) > ANumber(0))
  {
    iRet++;
    std::cerr << "ANumber::operator > failed: -1>0" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if (!(ANumber(0) >= ANumber(0)))
  {
    iRet++;
    std::cerr << "ANumber::operator >= failed: ! 0>=0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(-1) > ANumber(0))
  {
    iRet++;
    std::cerr << "ANumber::operator > failed: -1>0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(-1) > ANumber(1))
  {
    iRet++;
    std::cerr << "ANumber::operator > failed: -1>1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (!(ANumber(1) > ANumber(-1)))
  {
    iRet++;
    std::cerr << "ANumber::operator > failed: 1>-1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(1) == ANumber(-1))
  {
    iRet++;
    std::cerr << "ANumber::operator == failed: 1==-1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(-1) == ANumber(1))
  {
    iRet++;
    std::cerr << "ANumber::operator == failed: -1==1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber(1) == ANumber(0))
  {
    iRet++;
    std::cerr << "ANumber::operator == failed: 1==0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (!(ANumber(1) != ANumber(0)))
  {
    iRet++;
    std::cerr << "ANumber::operator != failed: ! 1!=0" << std::endl;
  } else std::cerr << "." << std::flush;


  if (!(ANumber("0.1") > ANumber(-1)))
  {
    iRet++;
    std::cerr << "ANumber::operator > failed: ! 0.1>-1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber("0.1") == ANumber("-0.1"))
  {
    iRet++;
    std::cerr << "ANumber::operator == failed: 0.1 == -0.1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (ANumber("-0.1") <= ANumber(-1))
  {
    iRet++;
    std::cerr << "ANumber::operator <= failed: -0.1<=-1" << std::endl;
  } else std::cerr << "." << std::flush;

  if (!(ANumber("-0.1") >= ANumber(-1)))
  {
    iRet++;
    std::cerr << "ANumber::operator <= failed: ! -0.1>=-1" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testRounding(int& iRet)
{
  ANumber num("1.99");

  num.forcePrecision(1);
  if (num.toAString().compare("2.0"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(1) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  num.forcePrecision(0);
  if (num.toAString().compare("2"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(0) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  num.forcePrecision(5);
  if (num.toAString().compare("2.00000"))
  {
    iRet++;
    std::cerr << "ANumber::forcePrecision(5) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  num.setNumber("3.1415");
  num.forceWhole();
  if (num.toAString().compare("3"))
  {
    iRet++;
    std::cerr << "ANumber::forceWhole failed" << std::endl;
  } else std::cerr << "." << std::flush;

}

int ut_ANumber_General(void)
{
  std::cerr << "ut_ANumber_General" << std::endl;

	int iRet = 0x0;

//a_Assignment
//////////////////////////////////////////////////
  _utTestSetNumber(iRet);
  NEWLINE_UNIT_TEST();

//a_Operator tests
//////////////////////////////////////////////////////////
  _testUnaryLinearOperators(iRet);
  NEWLINE_UNIT_TEST();
  _testUnaryMultiplicativeOperators(iRet);
  NEWLINE_UNIT_TEST();
  _testDivision(iRet);
  NEWLINE_UNIT_TEST();
  _testUnaryNonLinearOperators(iRet);
  NEWLINE_UNIT_TEST();
  _testBinaryOperators(iRet);
  NEWLINE_UNIT_TEST();
  _testComparison(iRet);
  NEWLINE_UNIT_TEST();
  _testTests(iRet);
  NEWLINE_UNIT_TEST();

//a_Casting/conversion tests
///////////////////////////////////////////////////////////
  _testRounding(iRet);
  NEWLINE_UNIT_TEST();
  _testPrecision(iRet);

  return iRet;
}

