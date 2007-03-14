#include "pchUnitTests.hpp"

int ut_AString_Compare()
{
  std::cerr << "ut_AString_Compare" << std::endl;
  
  int iRet = 0x0;
  
  AString str0 = "Same thing", str1;
  str1.assign("Same thing");
  
  //a_Inequality check
  ASSERT_UNIT_TEST(!(str0 != str1), "operator !=", "0", iRet);

  //a_Equality check
  str0.clear();
  ASSERT_UNIT_TEST(!(str0 == str1), "operator ==", "0", iRet);

  //a_Less that
  str0 = "0123";
  str1 = "012";
  ASSERT_UNIT_TEST(!(str0 < str1), "operator <", "0", iRet);

  //a_Greater than
  str0 = "ABC";
  str1 = "ABCD";
  ASSERT_UNIT_TEST(!(str0 > str1), "operator >", "0", iRet);

  //a_Less that or equal
  str0 = "123";
  str1 = "012";
  ASSERT_UNIT_TEST(!(str0 <= str1), "operator <=", "0", iRet);

  str0 = "ABCD";
  str1 = "ABC";
  ASSERT_UNIT_TEST(!(str0 <= str1), "operator <=", "1", iRet);

  str0 = "xyz";
  str1 = "xyz";
  ASSERT_UNIT_TEST((str0 <= str1), "operator <=", "2", iRet);

  //a_Greater than
  str0 = "ABC";
  str1 = "BCD";
  ASSERT_UNIT_TEST(!(str0 >= str1), "operator >=", "1", iRet);

  str0 = "ABC";
  str1 = "ABCD";
  ASSERT_UNIT_TEST(!(str0 >= str1), "operator >=", "1", iRet);

  str0 = "zyx";
  str1 = "zyx";
  ASSERT_UNIT_TEST((str0 >= str1), "operator >=", "2", iRet);

  //a_Compare no case
  str0 = "ABC";
  str1 = "abc";
  ASSERT_UNIT_TEST((str0.compareNoCase(str1) == 0x0), "compareNoCase", "0", iRet);

  //a_Compare with case
  str0 = "ABC";
  str1 = "ABc";
  ASSERT_UNIT_TEST((str0.compare(str1) != 0x0), "compare", "0", iRet);

  //a_Subset
  str0 = "ABC";
  str1 = "ABCD";
  ASSERT_UNIT_TEST((str0.compare(str1) != 0x0), "compare", "1", iRet);

  //a_diff
  str0 = "ABCE";
  str1 = "ABCDE";
  ASSERT_UNIT_TEST((str0.diff(str1) == 0x3), "diff", "0", iRet);

  str0 = "AB";
  str1 = "ABC";   //a_Contains str0, diff at position 2
  ASSERT_UNIT_TEST((str0.diff(str1) == 2), "diff", "1", iRet);

  str0 = "ABC";
  str1 = "";
  ASSERT_UNIT_TEST((str0.diff(str1) == 0x0), "diff", "2", iRet);

  str0 = "ABCD";
  str1 = "BCX";
  ASSERT_UNIT_TEST((str0.diff(str1, 0x1) == 0x3), "diff", "3", iRet);

  //a_equalsX
  str0.assign("AbCd", 4);
  str1 = "ABCD";
  ASSERT_UNIT_TEST(str0.equalsNoCase(str1), "equalsNoCase", "0", iRet);

  str0.assign("AbCd", 4);
  str1 = "AbCd";
  ASSERT_UNIT_TEST(str1.equals(str0), "equals", "1", iRet);

  return iRet;
}
