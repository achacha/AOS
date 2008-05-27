/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include "AFragmentString.hpp"

int ut_AFragmentString_General(void)
{
  int iRet = 0x0;

  std::cerr << "ut_AFragmentString_General" << std::endl;

	AFragmentString fstr("foo-[1c]\\\\(4,3,1)");
	AString str;
  fstr.next(str);
	int i;
  ASSERT_UNIT_TEST(fstr.getPermutations() == 6, "AFragmentString getPermutations", "0", iRet);

  for (i=0; i<4; ++i) {
		str.clear();
    fstr.next(str);
	}
  ASSERT_UNIT_TEST(str.equals("foo-c\\0002"), "AFragmentString next", "0", iRet);

  //
	fstr.clear();
	fstr.parse("<4>");
  ASSERT_UNIT_TEST(fstr.getPermutations() == 10000, "AFragmentString getPermutations", "1", iRet);

  //
	fstr.clear();
	fstr.parse("(2)");
  ASSERT_UNIT_TEST(fstr.getPermutations() == 100, "AFragmentString getPermutations", "2", iRet);

	//
  fstr.clear();
	fstr.parse("(4,100,112,-3)");
  ASSERT_UNIT_TEST(fstr.getPermutations() == 5, "AFragmentString getPermutations", "3", iRet);
  str.clear();
	fstr.next(str);
  ASSERT_UNIT_TEST(str.equals("0112"), "AFragmentString next", "1", iRet);
	for (i=0; i<2; ++i) {
		str.clear();
    fstr.next(str);
	}
  ASSERT_UNIT_TEST(str.equals("0106"), "AFragmentString next", "2", iRet);
	for (i=0; i<2; ++i) {
		str.clear();
    fstr.next(str);
	}
  ASSERT_UNIT_TEST(str.equals("0100"), "AFragmentString next", "3", iRet);
	
	return iRet;
}
