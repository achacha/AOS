#include "pchUnitTests.hpp"

#include <ANumber.hpp>
#include <AException.hpp>

void _testFactorial(int& iRet)
{
  if ( ANumber::factorial(ANumber(0)) != ANumber("1.0") )
  {
    iRet++;
    std::cerr << "factorial(0) != 1.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if ( ANumber::factorial(ANumber(1)) != ANumber("1.0") )
  {
    iRet++;
    std::cerr << "factorial(1) != 1.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if ( ANumber::factorial(ANumber(2)) != ANumber("2.0") )
  {
    iRet++;
    std::cerr << "factorial(2) != 2.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber::factorial(ANumber(3)) != ANumber("6.0") )
  {
    iRet++;
    std::cerr << "factorial(3) != 6.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ! (ANumber::factorial(ANumber(4)) == ANumber("24.0")) )
  {
    iRet++;
    std::cerr << "factorial(4) != 24.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ! (ANumber::factorial(ANumber(5)) == ANumber("120.0")) )
  {
    iRet++;
    std::cerr << "factorial(5) != 120.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ! (ANumber::factorial(ANumber(6)) == ANumber("720.0")) )
  {
    iRet++;
    std::cerr << "factorial(6) != 720.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber::factorial(ANumber(7)) != ANumber("5040.0") )
  {
    iRet++;
    std::cerr << "factorial(7) != 5040.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber::factorial(ANumber(8)) != ANumber("40320.0") )
  {
    iRet++;
    std::cerr << "factorial(8) != 40320.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber::factorial(ANumber(9)) != ANumber("362880.0") )
  {
    iRet++;
    std::cerr << "factorial(9) != 362880.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ! (ANumber::factorial(ANumber(10)) == ANumber("3628800.0")) )
  {
    iRet++;
    std::cerr << "factorial(10) != 3628800.0 failed" << std::endl;
  } else std::cerr << "." << std::flush;
}

void _testEulerNumber(int& iRet)
{
  std::cerr << std::endl;

  if ( ANumber::e(ANumber(1), 12).toAString().compare("2.718281828459") )
  {
    iRet++;
    std::cerr << "e of 12 digits is not 2.718281828459" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber::e(ANumber(1), 64).toAString().compare("2.7182818284590452353602874713526624977572470936999595749669676277") )
  {
    iRet++;
    std::cerr << "e of 64 digits is not 2.7182818284590452353602874713526624977572470936999595749669676277" << std::endl;
  } else std::cerr << "." << std::flush;

//  std::cerr << "EulerNumber(2, 12) == " << ANumber::e(ANumber(2), 12).toAString() << " should be ~7.389056098931" << std::endl;
//  std::cerr << "EulerNumber(3, 12) == " << ANumber::e(ANumber(3), 11).toAString() << " should be ~20.08553692319" << std::endl;
//  std::cerr << "EulerNumber(4, 12) == " << ANumber::e(ANumber(4), 12).toAString() << " should be ~54.59815003314" << std::endl;
//  std::cerr << "EulerNumber(5, 12) == " << ANumber::e(ANumber(5), 10).toAString() << " should be ~148.4131591026" << std::endl;
//  std::cerr << "EulerNumber(6, 12) == " << ANumber::e(ANumber(6), 10).toAString() << " should be ~403.4287934927" << std::endl;
//  std::cerr << "EulerNumber(7, 12) == "  << ANumber::e(ANumber(7), 9).toAString() <<  " should be ~1096.633158428" << std::endl;
//  std::cerr << "EulerNumber(8, 12) == "  << ANumber::e(ANumber(8), 9).toAString() <<  " should be ~2980.957987042" << std::endl;
//  std::cerr << "EulerNumber(9, 12) == "  << ANumber::e(ANumber(9), 9).toAString() <<  " should be ~8103.083927576" << std::endl;
//  std::cerr << "EulerNumber(10, 12) == " << ANumber::e(ANumber(10), 8).toAString() << " should be ~22026.46579481" << std::endl;
}

void _testTrig(int& iRet)
{
  std::cerr << std::endl;

  if ( ANumber::sin(ANumber("1"), 13).toAString().compare("0.8414709848079") )
  {
    iRet++;
    std::cerr << "sin(1)" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber::cos(ANumber("1"), 13).toAString().compare("0.5403023058681") )
  {
    iRet++;
    std::cerr << "cos(1) failed" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Roughly zero
  ANumber nX = ANumber::sin(ANumber("3.141592654"), 13);
  if ( ! (nX < ANumber("0.000000001") && nX > ANumber("-0.000000001")) )
  {
    iRet++;
    std::cerr << "sin(~Pi) failed: " << nX.toAString().c_str() << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Roughly -1
  nX = ANumber::cos(ANumber("3.141592654"), 13);
  if ( ! (nX < ANumber("-0.999999999")) )
  {
    iRet++;
    std::cerr << "cos(~Pi) failed: " << nX.toAString().c_str() << std::endl;
  } else std::cerr << "." << std::flush;

}

void _testPrime(int& iRet)
{
  if ( ! ANumber("13").isPrime())
  {
    iRet++;
    std::cerr << "PrimeTest for 13 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber("24").isPrime())
  {
    iRet++;
    std::cerr << "PrimeTest for 24 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ! ANumber("0").isPrime())
  {
    iRet++;
    std::cerr << "PrimeTest for 0 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber("69").isPrime())
  {
    iRet++;
    std::cerr << "PrimeTest for 69 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  if ( ANumber("10").isPrime())
  {
    iRet++;
    std::cerr << "PrimeTest for 10 failed" << std::endl;
  } else std::cerr << "." << std::flush;
}

//a_Using: The Miraculous Bailey-Borwein-Plouffe Pi Algorithm
void _generatePiDigits(ANumber &nR, int digits)
{
//  std::cerr << "\r\n_________________Generating Pi digits_______________\r\n";

  if (digits <= 0)
  {
    std::cerr << "It's 3.14, now go away and don't comeback without a decent digit count!" << std::endl;
    return;
  }

  digits += 3;   //a_Rounding error buffer

  //a_nD    - difference terms
  //a_nP    - product term
  //a_nT    - iterator
  //a_nR    - result
  //a_nTerm - intermediate term
  ANumber nD[4], nP, nT(0), nTerm;
  nR = 0;
  nR.setPrecision(digits);
  nT.setPrecision(digits);

  //a_A little trick to force precision and accelerate calculations
  ANumber nn[10], nn16r;
  for (int iX = 0x0; iX < 9; iX++)
  {
    nn[iX] = iX;
    nn[iX].setPrecision(digits);
  }
  nn16r.setPrecision(digits);
  nn16r = 1;
  nn16r /= ANumber(16);
  
  for (;; nT++)
  {
    ANumber nX(nn16r);
    nD[0] = (nn[4] / (nn[8] * nT + nn[1]));
    nD[1] = (nn[2] / (nn[8] * nT + nn[4]));
    nD[2] = (nn[1] / (nn[8] * nT + nn[5]));
    nD[3] = (nn[1] / (nn[8] * nT + nn[6]));
    nP = nX.power(nT);

    nTerm = (nD[0] - nD[1] - nD[2] - nD[3]) * nP;
//    std::cerr << nT.toAString() << ": " << nTerm.toAString() << " == ";

    if (nTerm.isZero()) break;

    nR += nTerm;
//    std::cerr << nR.toAString() << std::endl;
  }

  digits -= 3;
  nR.setPrecision(digits);

//  std::cerr << "Pi(" << digits << ") == " << nR.toAString() << std::endl;
  std::cerr << "." << std::flush;   //a_Show some activity
}

//a_50 digits of Pi
const char *pccPi = "3.\
14159265358979323846264338327950288419716939937510\
";

//a_120 digits of Pi
//const char *pccPi = "3.\
//141592653589793238462643383279502884197169399375105820974944\
//592307816406286208998628034825342117067982148086513282306647\
//";

//360 digits of Pi
//const char *pccPi = "3.\
//141592653589793238462643383279502884197169399375105820974944\
//592307816406286208998628034825342117067982148086513282306647\
//093844609550582231725359408128481117450284102701938521105559\
//644622948954930381964428810975665933446128475648233786783165\
//271201909145648566923460348610454326648213393607260249141273\
//724587006606315588174881520920962829254091715364367892590360\
//";


void _checkPiDigits(int& iRet)
{
  std::cerr << std::endl;

  ANumber nPi;
  u4 digits = strlen(pccPi);
	assert(digits >= 2);
	digits -= 2;
  
	_generatePiDigits(nPi, digits);  //a_Length of reference without "3."

  //a_Due to the fact we do not go to HUGE precision to generate Pi, the last
  //a_ digit may be incorrect.  This is becauuse the const char* I have defined
  //a_ above was generated by a super-computer working with gigantic precison and
  //a_ enormous amount of non-pertinent terms which may have eventually added up to
  //a_ throw us off just a little bit, also, we used 3 digit roundoff error which
  //a_ may have affected the 100th term.  For 50 digit test we are off by the last
  //a_ digit, we have 1, the standard is 0, so we went a bit over (1/10^50) due to 
  //a_ this roundoff/limited precision calculation. C'est la vie :)
  //a_You may uncomment the higher precision Pi's above, but the calculation time
  //a_ is very very very loooooooooooooooooong!
//  std::cerr << "Pi Test for " << digits << " digits" << std::endl;
//  std::cerr << "*** Actual  ***" << std::endl << pccPi << std::endl;
//  std::cerr << "*** We have ***" << std::endl << nPi.toAString() << std::endl;

  //a_Within one digit
  if (nPi.toAString().diff(pccPi) < (digits - 0x1))
  {
    iRet++;
    std::cerr << "Pi generation failed" << std::endl;
  } else std::cerr << "." << std::flush;

}

void _testSquareRoot(int& iRet)
{
  ANumber n0("3");
  n0.setPrecision(40);
  n0.sqrt();
  //a_1.7320508075688772935274463415058723669428
  if (n0.toAString().compare("1.7320508075688772935274463415058723669428"))
  {
    iRet++;
    std::cerr << "sqrt of 3 failed" << std::endl;
  } else std::cerr << "." << std::flush;

  n0.setNumber("2");
  n0.setPrecision(40);
  n0.sqrt();
  //a_1.4142135623730950488016887242096980785697
  if (n0.toAString().compare("1.4142135623730950488016887242096980785697"))
  {                          
    iRet++;
    std::cerr << "sqrt of 2 failed: " << n0.toAString() << " != 1.4142135623730950488016887242096980785697" << std::endl;
  } else std::cerr << "." << std::flush;

  n0.setNumber("256");
  n0.sqrt();
  //a_16
  if (n0.toAString().compare("16"))
  {
    iRet++;
    std::cerr << "sqrt of 256 failed" << std::endl;
  } else std::cerr << "." << std::flush;


  n0.setNumber("3.1415965429");
  n0.setPrecision(40);
  n0.sqrt();
  //a_1.7724549480593293770975103015744526049594
  if (n0.toAString().compare("1.7724549480593293770975103015744526049594"))
  {
    iRet++;
    std::cerr << "sqrt of 3.14.4965429 failed: " << n0.toAString() << " != 1.7724549480593293770975103015744526049594" << std::endl;
  } else std::cerr << "." << std::flush;


}

int ut_ANumber_Functionality(void)
{
  int iRet = 0x0;

  std::cerr << "ut_ANumber_Functionality (takes 19 seconds on P2-200)" << std::endl;

//a_Functionality tests
///////////////////////////////////////////////////////////
  _testFactorial(iRet);
  _testEulerNumber(iRet);
  _testTrig(iRet);
  _testPrime(iRet);
  _checkPiDigits(iRet);
  _testSquareRoot(iRet);

  return iRet;
}

