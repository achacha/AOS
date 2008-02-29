#include "pchUnitTests.hpp"

void testBasics(int &iRet)
{
  AString str0 = "reverse engineer";
  AString str1(str0);
  str0 = "reenigne esrever";

  //a_reverse test
  str1.reverse();
  ASSERT_UNIT_TEST(str1 == str0, "AString::reverse", "0", iRet);

  str0.clear();
  str0.reverse();     //a_Reverse an empty string should do nothing
  str1.reverse();
  ASSERT_UNIT_TEST(str1 != str0, "AString::reverse", "1", iRet);

  //a_toLower test
  str0.assign("she atacked and insulted you far from fare in the end");
  str1 = "She ATAcked aNd InSulted YOU faR FRom fare In the END";
  str1.makeLower();
  ASSERT_UNIT_TEST(str1 == str0, "AString::makeLower", "0", iRet);

  str0.clear();
  str0.makeLower();
  ASSERT_UNIT_TEST(str1 != str0, "AString::makeLower", "1", iRet);

  //a_toUpper test
  str0.assign("HERE WILL WE BE SILLY AND RID THE SEED SOMEONE SEW");
  str1 = "heRE WIll wE BE SillY AND riD THE seED soMEonE SEW";
  str1.makeUpper();
  ASSERT_UNIT_TEST(str1 == str0, "AString::makeUpper", "0", iRet);

  str0.clear();
  str0.makeUpper();
  ASSERT_UNIT_TEST(str1 != str0, "AString::makeUpper", "1", iRet);
}

void testConversion(int& iRet)
{
  AString str0, str1;

  //a_Pointer conversions
  str0 = "0x400";
  void *p0 = (void *)0x400;
  ASSERT_UNIT_TEST(str0 == AString::fromPointer(p0), "AString::fromPointer", "0", iRet);

  //a_conversion
  str0 = "654321";
  s4 l0 = 654321;
  ASSERT_UNIT_TEST(str0 == AString::fromS4(l0), "AString::fromS4", "0", iRet);

  str0 = "-12345";
  l0 = -12345;
  ASSERT_UNIT_TEST(str0 == AString::fromS4(l0), "AString::fromS4", "1", iRet);

  str1 = "4294954951";
  ASSERT_UNIT_TEST(str1 == AString::fromU4(l0), "AString::fromU4", "0", iRet);

  l0 = 0x2020;
  str1 = "32";
  ASSERT_UNIT_TEST(str1 == AString::fromS1((s1)l0), "AString::fromS1", "0", iRet);

  str1 = "2020";
  ASSERT_UNIT_TEST(str1 == AString::fromU2((s2)l0, 0x10), "AString::fromU2", "0", iRet);

  //a_Int conversion
  str0 = "65535";
  int i0 = 65535;
  ASSERT_UNIT_TEST(str0 == AString::fromInt(i0), "AString::fromInt", "0", iRet);

  str0 = "-32766";
  i0 = -32766;
  ASSERT_UNIT_TEST(str0 == AString::fromInt(i0), "AString::fromInt", "1", iRet);

  str0 = "0";
  i0 = 0;
  ASSERT_UNIT_TEST(str0 == AString::fromInt(i0), "AString::fromInt", "2", iRet);

  str0 = "999999";
  size_t ii1 = 999999;
  ASSERT_UNIT_TEST(str0 == AString::fromSize_t(ii1), "AString::fromSize_t", "0", iRet);
  ASSERT_UNIT_TEST(ii1 == str0.toSize_t(), "AString::toSize_t", "0", iRet);

  ASSERT_UNIT_TEST(ASWNL("tRuE").toBool(), "AString::toBool", "0", iRet);
  ASSERT_UNIT_TEST(ASWNL("YeS").toBool(), "AString::toBool", "1", iRet);
  ASSERT_UNIT_TEST(ASWNL("1").toBool(), "AString::toBool", "2", iRet);
  ASSERT_UNIT_TEST(!ASWNL("tRu").toBool(), "AString::toBool", "3", iRet);
  ASSERT_UNIT_TEST(!ASWNL("").toBool(), "AString::toBool", "4", iRet);
  ASSERT_UNIT_TEST(!ASWNL("false").toBool(), "AString::toBool", "3", iRet);
  ASSERT_UNIT_TEST(!ASWNL("0").toBool(), "AString::toBool", "3", iRet);
}

void testTrim(int& iRet)
{
  //a_Trimming routines
  AString str0("ABCDEFG");
  str0.stripLeading("CBA");
  ASSERT_UNIT_TEST(!str0.compare("DEFG"), "AString::stripLeading", "0", iRet);
  
  str0 = ".&.A.4.1.";
  str0.stripLeading(".&%@.41AB");
  ASSERT_UNIT_TEST(str0.isEmpty(), "AString::stripLeading", "1", iRet);

  str0 = "ABC123";
  str0.stripLeading(" ");
  ASSERT_UNIT_TEST(str0 == "ABC123", "AString::stripLeading", "2", iRet);

  str0 = " ";
  str0.stripLeading(" ");
  ASSERT_UNIT_TEST(str0.isEmpty(), "AString::stripLeading", "3", iRet);

  str0 = "1";
  str0.stripLeading(" ");
  ASSERT_UNIT_TEST(str0 == "1", "AString::stripLeading", "4", iRet);

  str0 = "12\t3.4 5     .  \t  ";
  str0.stripTrailing(" .\t");
  ASSERT_UNIT_TEST(!str0.compare("12\t3.4 5"), "AString::stripTrailing", "0", iRet);

  str0 = ".&.A.4.1.";
  str0.stripTrailing(".&%@.41AB");
  ASSERT_UNIT_TEST(str0.isEmpty(), "AString::stripTrailing", "1", iRet);

  str0 = " ";
  str0.stripTrailing(" ");
  ASSERT_UNIT_TEST(str0.isEmpty(), "AString::stripTrailing", "2", iRet);

  str0 = "ABC123";
  str0.stripTrailing(" ");
  ASSERT_UNIT_TEST(str0 == "ABC123", "AString::stripTrailing", "3", iRet);

  str0 = "1";
  str0.stripTrailing(" ");
  ASSERT_UNIT_TEST(str0 == "1", "AString::stripTrailing", "4", iRet);

  str0 = "172.20.24.34";
  str0.stripEntire(".");
  ASSERT_UNIT_TEST(str0 == "172202434", "AString::stripEntire", "0", iRet);

  str0 = "How now brown cow?";
  str0.stripEntire("ow");
  ASSERT_UNIT_TEST(str0 == "H n brn c?", "AString::stripEntire", "1", iRet);
}

void testSplit(int& iRet)
{
  LIST_AString names;
  AString str0("path0/path1/path2");
  str0.split(names, '/');
  ASSERT_UNIT_TEST(names.size() == 3, "AString::split", "0", iRet);
  
  LIST_AString::iterator it = names.begin();
  ASSERT_UNIT_TEST(!(*it).compare("path0"), "AString::split", "1", iRet);

  ++it;
  ASSERT_UNIT_TEST(!(*it).compare("path1"), "AString::split", "2", iRet);

  ++it;
  ASSERT_UNIT_TEST(!(*it).compare("path2"), "AString::split", "3", iRet);

  ++it;
  ASSERT_UNIT_TEST(it == names.end(), "AString::split", "3a", iRet);

  str0.assign("\\\\\\\\path0\\\\\\path1\\\\path2\\");
  names.clear();
  str0.split(names, '\\');
  ASSERT_UNIT_TEST(names.size() == 3, "AString::split", "4", iRet);

  it = names.begin();
  ASSERT_UNIT_TEST(!(*it).compare("path0"), "AString::split", "5", iRet);

  ++it;
  ASSERT_UNIT_TEST(!(*it).compare("path1"), "AString::split", "6", iRet);

  ++it;
  ASSERT_UNIT_TEST(!(*it).compare("path2"), "AString::split", "7", iRet);
  
  ++it;
  ASSERT_UNIT_TEST(it == names.end(), "AString::split", "8", iRet);

  str0.assign("_path0_path1_path2");
  names.clear();
  str0.split(names, '_');
  ASSERT_UNIT_TEST(names.size() == 3, "AString::split", "9", iRet);
  
  it = names.begin();
  ASSERT_UNIT_TEST(!(*it).compare("path0"), "AString::split", "10", iRet);

  ++it;
  ASSERT_UNIT_TEST(!(*it).compare("path1"), "AString::split", "11", iRet);

  ++it;
  ASSERT_UNIT_TEST(!(*it).compare("path2"), "AString::split", "12", iRet);

  ++it;
  ASSERT_UNIT_TEST(it == names.end(), "AString::split", "13", iRet);
}

void testWrap(int& iRet)
{
  const char *pcc = "Test wrapper";
  const AString& temp = AString::wrap(pcc);
  ASSERT_UNIT_TEST(temp.c_str() == pcc, "AString::wrap", "0", iRet);
  ASSERT_UNIT_TEST(!temp.compareNoCase("test WRaPPer"), "AString::wrap", "1", iRet);
}

void testJustify(int& iRet)
{
  AString str;
  str.assign("2f8");
  str.justifyRight(4, '0');
  ASSERT_UNIT_TEST(str.equals("02f8"), "AString::justifyRight", "0", iRet);
  
  str.justifyRight(8, '0');
  ASSERT_UNIT_TEST(str.equals("000002f8"), "AString::justifyRight", "1", iRet);

  str.justifyRight(3, '0');
  ASSERT_UNIT_TEST(str.equals("000002f8"), "AString::justifyRight", "2", iRet);

  str.assign("2f8");
  str.justifyCenter(8, '0');
  ASSERT_UNIT_TEST(str.equals("0002f800"), "AString::justifyCenter", "0", iRet);

  str.justifyCenter(11, '0');
  ASSERT_UNIT_TEST(str.equals("000002f8000"), "AString::justifyCenter", "1", iRet);

  str.assign("2f8");
  str.justifyCenter(11, '0');
  ASSERT_UNIT_TEST(str.equals("00002f80000"), "AString::justifyCenter", "2", iRet);

  str.justifyCenter(5, '0');
  ASSERT_UNIT_TEST(str.equals("00002f80000"), "AString::justifyCenter", "3", iRet);

  str.assign("2f8");
  str.justifyLeft(7, '0');
  ASSERT_UNIT_TEST(str.equals("2f80000"), "AString::justifyLeft", "0", iRet);

  str.justifyLeft(5, '0');
  ASSERT_UNIT_TEST(str.equals("2f80000"), "AString::justifyLeft", "1", iRet);
}

int ut_AString_Utility()
{
  std::cerr << "ut_AString_Utility" << std::endl;

  int iRet = 0x0;

  testBasics(iRet);
  testConversion(iRet);
  testTrim(iRet);
  testSplit(iRet);
  testWrap(iRet);
  testJustify(iRet);

  return iRet;
}

