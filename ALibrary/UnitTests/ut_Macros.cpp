/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include "AString.hpp"

int ut_Macros()
{
  std::cerr << "ut_Macros" << std::endl;

  int iRet = 0;
  u4 dw = MAKE_U4(0xCA, 0xFE, 0xBA, 0xBE);
  ASSERT_UNIT_TEST((dw == 0xCAFEBABE), "MAKE_U4", "0", iRet);

  SET_U4_0(dw, 0x0D);
  ASSERT_UNIT_TEST((dw == 0xCAFEBA0D), "SET_U4_0", "0", iRet);
  SET_U4_1(dw, 0xF0);
  ASSERT_UNIT_TEST((dw == 0xCAFEF00D), "SET_U4_1", "1", iRet);
  SET_U4_2(dw, 0xAD);
  ASSERT_UNIT_TEST((dw == 0xCAADF00D), "SET_U4_2", "2", iRet);
  SET_U4_3(dw, 0xBA);
  ASSERT_UNIT_TEST((dw == 0xBAADF00D), "SET_U4_3", "3", iRet);

  return iRet;
}