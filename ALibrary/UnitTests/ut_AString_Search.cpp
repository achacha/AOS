/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"

void testSearchBasic(int& iRet)
{
  AString str0 = "This is my test!";

  //a_Forward finds
  if (str0.find('!') != 15)
  {
    iRet++;
    std::cerr << str0 << "does not contain '!'" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if (str0.find("This") != 0)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"This\"" << std::endl;
  } else std::cerr << "." << std::flush;

  AString str1 = "my";
  if (str0.find(str1) != 8)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"my\" at position 8" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.find("test!!") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains \"test!!\"" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.find("Very long string that is definitely not in the source") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains <long string>" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Forward find with index
  str0 = "0123Me6MeMe";
  str1.assign("Me");
  u4 iPos = 0x0;
  if ((iPos = str0.find(str1, iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find(str1, iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find(str1, iPos + 1)) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 9" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find(str1, iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;


  //a_More forward find with index
  str0 = "0123M56M8M0";
  iPos = 0x0;
  if ((iPos = str0.find('M', iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find('M', iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find('M', iPos + 1)) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 9" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.find('M', iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;


  //a_findIn
  str0 = "0123456789";
  str1 = "478";
  iPos = 0x0;
  if ((iPos = str0.findOneOf(str1, iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findOneOf(str1, iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findOneOf(str1, iPos + 1)) != 8)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 8" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findOneOf(str1, iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain one of " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;

  //a_findNotIn
  str0 = "0123456789";
  str1 = "0123569";
  iPos = 0x0;
  if ((iPos = str0.findNotOneOf(str1, iPos)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 4" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findNotOneOf(str1, iPos + 1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findNotOneOf(str1, iPos + 1)) != 8)
  {
    iRet++;
    std::cerr << str0 << "does not contain one of " << str1 << " at position 8" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.findNotOneOf(str1, iPos + 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain one of " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Reverse finds
  str0 = "!tset si 0123 is test!";
  if (str0.rfind('!') != 21)
  {
    iRet++;
    std::cerr << str0 << "does not contain '!' at 21" << std::endl;
  } else std::cerr << "." << std::flush;
  
  if (str0.rfind('!', 3) != 0)
  {
    iRet++;
    std::cerr << str0 << "does not contain '!' at 0" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("test") != 17)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"test\"" << std::endl;
  } else std::cerr << "." << std::flush;

  str1 = "0123";
  if (str0.rfind(str1) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain \"0123\" at position 9" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("test!!") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains \"test!!\"" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("Very long string that is bigger than the source") != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains <a very long string>" << std::endl;
  } else std::cerr << "." << std::flush;

  if (str0.rfind("Something", 0) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contains 'Something' before 0" << std::endl;
  } else std::cerr << "." << std::flush;

  //a_Reverse find with index
  str0 = "0123Me6MeMe";
  str1.assign("Me");
  if ((iPos = str0.rfind(str1, AConstant::npos)) != 9)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 10" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.rfind(str1, iPos -1)) != 7)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 7" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.rfind(str1, iPos - 1)) != 4)
  {
    iRet++;
    std::cerr << str0 << "does not contain " << str1 << " at position 9" << std::endl;
  } else std::cerr << "." << std::flush;
  if ((iPos = str0.rfind(str1, iPos - 1)) != AConstant::npos)
  {
    iRet++;
    std::cerr << str0 << "does contain " << str1 << " at position " << iPos << std::endl;
  } else std::cerr << "." << std::flush;
}

void testReverse(int& iRet)
{
  AString str("somefile.ext..ext1.ext2");
  size_t pos = str.rfind('.');
  ASSERT_UNIT_TEST(pos == 18, "AString::rfind", "0", iRet);

  pos = str.rfind(':');
  ASSERT_UNIT_TEST(pos == AConstant::npos, "AString::rfind", "1", iRet);

  str.rremoveUntil('.');
  ASSERT_UNIT_TEST(str.equals(ASWNL("somefile.ext..ext1")), "AString::rremoveUntil", "0", iRet);

  str.rremoveUntilOneOf(ASW(".;:",3));
  ASSERT_UNIT_TEST(str.equals(ASWNL("somefile.ext")), "AString::rremoveUntilOneOf", "0", iRet);

  str.rremove(4);
  ASSERT_UNIT_TEST(str.equals(ASWNL("somefile")), "AString::rremove", "0", iRet);
}

void testStringPartial(int& iRet)
{
  AString str("foo/bar");
  ASSERT_UNIT_TEST(str.startsWith(ASWNL("foo")), "AString::startsWith", "0", iRet);
  ASSERT_UNIT_TEST(str.startsWithNoCase(ASWNL("Foo")), "AString::startsWith", "1", iRet);
  ASSERT_UNIT_TEST(str.startsWith(ASWNL("f")), "AString::startsWith", "2", iRet);
  ASSERT_UNIT_TEST(str.endsWith(ASWNL("bar")), "AString::endsWith", "0", iRet);
  ASSERT_UNIT_TEST(str.endsWithNoCase(ASWNL("O/BaR")), "AString::endsWith", "1", iRet);
  ASSERT_UNIT_TEST(str.endsWith(ASWNL("ar")), "AString::endsWith", "2", iRet);
}

void testPattern(int& iRet)
{
  AString base("HTTP/1.0 200 Ok?");

  ASSERT_UNIT_TEST(base.matchPattern(ASWNL("*/1.? 200")), "AString::matchPattern", "0", iRet);
  ASSERT_UNIT_TEST(base.matchPattern(ASWNL("*200")), "AString::matchPattern", "1", iRet);
  ASSERT_UNIT_TEST(base.matchPattern(ASWNL("H??P")), "AString::matchPattern", "2", iRet);
  ASSERT_UNIT_TEST(base.matchPattern(ASWNL("??")), "AString::matchPattern", "3", iRet);
  ASSERT_UNIT_TEST(base.matchPattern(ASWNL("*")), "AString::matchPattern", "4", iRet);
  ASSERT_UNIT_TEST(base.matchPattern(ASWNL("*\\?")), "AString::matchPattern", "5", iRet);

  ASSERT_UNIT_TEST(!base.matchPattern(ASWNL("?f")), "AString::matchPattern", "6", iRet);
  ASSERT_UNIT_TEST(!base.matchPattern(ASWNL("*z")), "AString::matchPattern", "7", iRet);
  ASSERT_UNIT_TEST(!base.matchPattern(ASWNL("HTTP/1.? 2?? Ok\?X")), "AString::matchPattern", "8", iRet);

  ASSERT_UNIT_TEST(4 == base.findPattern(ASWNL("/?.? 200"), 2), "AString::findPattern", "0", iRet);
  ASSERT_UNIT_TEST(AConstant::npos == base.findPattern(ASWNL("/?.? 200"), 5), "AString::findPattern", "1", iRet);
  ASSERT_UNIT_TEST(AConstant::npos == base.findPattern(ASWNL("/?.? 200"), 11), "AString::findPattern", "2", iRet);
}

int ut_AString_Search()
{
  std::cerr << "ut_AString_Search" << std::endl;

  int iRet = 0x0;

  testSearchBasic(iRet);
  testReverse(iRet);
  testStringPartial(iRet);
  testPattern(iRet);

  return iRet;
}

