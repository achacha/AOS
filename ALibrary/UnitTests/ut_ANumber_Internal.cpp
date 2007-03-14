#include "pchUnitTests.hpp"

#include <ANumber.hpp>
#include <AException.hpp>


#ifdef __UT_ANUMBER_INTERNAL__
//=====================================================================================
void _internalAdd()
{
  std::cerr << std::endl << "********  _internalAdd" << std::endl;

  ANumber nX;
  AString strA = "999999",
         strB = "1",
         strR;

  nX._strAdd(strA, strB, strR);
  std::cerr << strA << " + " << strB << " = " << strR << " should be 1000000" << std::endl;

  strA = "299"; strB = "3";
  nX._strAdd(strA, strB, strR);
  std::cerr << strA << " + " << strB << " = " << strR << " should be 302" << std::endl;
}

void _internalSubtract()
{
  std::cerr << std::endl << "********  _internalSubtract" << std::endl;

  ANumber nX;
  AString strA = "13",
         strB = "6",
         strR;

  int iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be +7" << std::endl;

  strA = "1"; strB = ".13";
  iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be +0.87" << std::endl;

  strA = ".666"; strB = "1";
  iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be -0.334" << std::endl;

  strA = "0.666"; strB = "0.13";
  iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be +0.563" << std::endl;

  strA = "0"; strB = "666";
  iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be -666" << std::endl;

  strA = "6"; strB = "13";
  iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be -7" << std::endl;

  strA = "999999"; strB = "999333";
  iNegative = nX._strSubtract(strA, strB, strR);
  std::cerr << strA << " - " << strB << " = " << (iNegative ? "-" : "+") << strR << " should be +666" << std::endl;
}

void _internalCompare(void)
{
  std::cerr << std::endl << "********  _internalCompare" << std::endl;

  ANumber nX;
  AString strA = "13",
         strB = "6",
         strR;

  int iX = nX._strCompare(strA, strB);
  std::cerr << strA << " compare " << strB << " = " << iX << " should be 1" << std::endl;

  strA = "13"; strB = "13";
  iX = nX._strCompare(strA, strB);
  std::cerr << strA << " compare " << strB << " = " << iX << " should be 0" << std::endl;

  strA = "6"; strB = "13";
  iX = nX._strCompare(strA, strB);
  std::cerr << strA << " compare " << strB << " = " << iX << " should be -1" << std::endl;

  strA = "6"; strB = "1.3";
  iX = nX._strCompare(strA, strB);
  std::cerr << strA << " compare " << strB << " = " << iX << " should be 1" << std::endl;

  strA = "0.6"; strB = "1.3";
  iX = nX._strCompare(strA, strB);
  std::cerr << strA << " compare " << strB << " = " << iX << " should be -1" << std::endl;
}

void _internalMultiply(void)
{
  std::cerr << std::endl << "********  _internalMultiply" << std::endl;

  ANumber nX;
  AString strA = "2",
         strB = "3",
         strR;

  nX._strMultiply(strA, strB, strR);
  std::cerr << strA << " * " << strB << " = " << strR << " should be 6" << std::endl;

  strA = "0.5"; strB = "5";
  nX._strMultiply(strA, strB, strR);
  std::cerr << strA << " * " << strB << " = " << strR << " should be 2.5" << std::endl;

  strA = "0.5"; strB = "0.5";
  nX._strMultiply(strA, strB, strR);
  std::cerr << strA << " * " << strB << " = " << strR << " should be 0.25" << std::endl;

  strA = "666"; strB = "13";
  nX._strMultiply(strA, strB, strR);
  std::cerr << strA << " * " << strB << " = " << strR << " should be 8658" << std::endl;

  strA = "13"; strB = "666";
  nX._strMultiply(strA, strB, strR);
  std::cerr << strA << " * " << strB << " = " << strR << " should be 8658" << std::endl;

  strA = "13"; strB = "0";
  nX._strMultiply(strA, strB, strR);
  std::cerr << strA << " * " << strB << " = " << strR << " should be 0" << std::endl;
}

void _internalPower(void)
{
  std::cerr << std::endl << "********  _internalPower" << std::endl;

  ANumber nX;
  AString strA = "2",
         strB = "5",
         strR;

  nX._strPower(strA, strB, strR);
  std::cerr << strA << " ^ " << strB << " = " << strR << " should be 32" << std::endl;

  strA = "666"; strB = "3";
  nX._strPower(strA, strB, strR);
  std::cerr << strA << " ^ " << strB << " = " << strR << " should be 295408296" << std::endl;

  strA = "13"; strB = "13";
  nX._strPower(strA, strB, strR);
  std::cerr << strA << " ^ " << strB << " = " << strR << " should be 302875106592253" << std::endl;
}

void _internalPow10(void)
{
  std::cerr << std::endl << "********  _internalPow10" << std::endl;

  ANumber nX;
  AString strA = "2";

  nX._strPow10(strA, 0);
  std::cerr << "2 * 10^0 = " << strA << " should be 2" << std::endl;

  nX._strPow10(strA, 5);
  std::cerr << "2 * 10^5 = " << strA << " should be 200000" << std::endl;

  strA = "0.13";
  nX._strPow10(strA, 2);
  std::cerr << "0.13 * 10^2 = " << strA << " should be 13" << std::endl;

  strA = "123";
  nX._strPow10(strA, -2);
  std::cerr << "123 * 10^-2 = " << strA << " should be 1.23" << std::endl;

  strA = "13";
  nX._strPow10(strA, -5);
  std::cerr << "13 * 10^-5 = " << strA << " should be 0.00013" << std::endl;

  strA = "1.3";
  nX._strPow10(strA, -3);
  std::cerr << "1.3 * 10^-3 = " << strA << " should be 0.0013" << std::endl;

  strA = "1.3";
  nX._strPow10(strA, -1);
  std::cerr << "1.3 * 10^-1 = " << strA << " should be 0.13" << std::endl;

  strA = "13";
  nX._strPow10(strA, -1);
  std::cerr << "13 * 10^-1 = " << strA << " should be 1.3" << std::endl;

  strA = "666";
  nX._strPow10(strA, -1);
  std::cerr << "666 * 10^-1 = " << strA << " should be 66.6" << std::endl;

  strA = "666";
  nX._strPow10(strA, -2);
  std::cerr << "666 * 10^-2 = " << strA << " should be 6.66" << std::endl;

  strA = "666";
  nX._strPow10(strA, -3);
  std::cerr << "666 * 10^-3 = " << strA << " should be 0.666" << std::endl;

  strA = "666";
  nX._strPow10(strA, -4);
  std::cerr << "666 * 10^-4 = " << strA << " should be 0.0666" << std::endl;
}

void _internalDivide(void)
{
  std::cerr << std::endl << "********  _internalDivide" << std::endl;

  ANumber nX;
  AString strA = "1",
         strB = "3",
         strR;

  nX._strDivide(strA, strB, strR, 3);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.3333" << std::endl;

  strA = "2";
  nX._strDivide(strA, strB, strR, 3);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.6666 (rounding done by / and /= operators)" << std::endl;

  strA = "1"; strB = "2";
  nX._strDivide(strA, strB, strR, 1);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.5" << std::endl;

  strA = "1"; strB = "1";
  nX._strDivide(strA, strB, strR, 15);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 1" << std::endl;

  strA = "1"; strB = "2";
  nX._strDivide(strA, strB, strR, 0);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 1" << std::endl;

  strA = "1"; strB = "3";
  nX._strDivide(strA, strB, strR, 0);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0" << std::endl;

  strA = "666"; strB = "3";
  nX._strDivide(strA, strB, strR, 0);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 222" << std::endl;

  strA = "6"; strB = "25000";
  nX._strDivide(strA, strB, strR, 5);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.00024" << std::endl;

  strA = "6"; strB = "25";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.24" << std::endl;

  strA = "6"; strB = "2.5";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 2.4" << std::endl;

  strA = "6"; strB = "0.25";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 24" << std::endl;

  strA = "600"; strB = "0.00025";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 2400000" << std::endl;

  strA = "60"; strB = "0.00025";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 240000" << std::endl;

  strA = "6"; strB = "0.00025";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 24000" << std::endl;

  strA = "0.06"; strB = "2.5";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.024" << std::endl;

  strA = "0.6"; strB = "0.00025";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 2400" << std::endl;

  strA = "0.06"; strB = "0.00025";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 240" << std::endl;

  strA = "0.006"; strB = "0.00025";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 24" << std::endl;

  strA = "13"; strB = "666";
  nX._strDivide(strA, strB, strR, 6);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.01952" << std::endl;

  strA = "0.666"; strB = "1.3";
  nX._strDivide(strA, strB, strR, 4);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.5123" << std::endl;

  strA = "0.666"; strB = "1.3";
  nX._strDivide(strA, strB, strR, 3);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.512" << std::endl;

  strA = "0.666"; strB = "1.3";
  nX._strDivide(strA, strB, strR, 2);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.51" << std::endl;

  strA = "0.666"; strB = "1.3";
  nX._strDivide(strA, strB, strR, 1);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0.5" << std::endl;

  strA = "0.666"; strB = "1.3";
  nX._strDivide(strA, strB, strR, 0);
  std::cerr << strA << " / " << strB << " = " << strR << " should be 0" << std::endl;
}
//=====================================================================================
#endif

int ut_ANumber_Internal(void)
{
  int iRet = 0x0;

  std::cerr << "ut_ANumber_Internal" << std::endl;

#ifdef __UT_ANUMBER_INTERNAL__
//a_To use these internal tests make the respective methods of ANumber public
/////////////////////////////////////////////////////////////////////////////////
  _internalCompare();
  _internalAdd();
  _internalSubtract();
  _internalMultiply();
  _internalPower();
  _internalPow10();
  _internalDivide();
#else
  std::cerr << "ut_ANumber_Internal: __UT_ANUMBER_INTERNAL__ not set, skipping test" << std::endl;
#endif

  return iRet;
}