/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"

int ut_AString_Modify()
{
  std::cerr << "ut_AString_Modify" << std::endl;

  int iRet = 0x0;
  
  AString str("here and here and here");
  AString str1("there");
  str.overwrite(0x0, 0x4, str1);
  str.overwrite(0xA, 0x4, str1, 0x5);
  str.overwrite(0x14, 0x4, str1, 0x5);
  ASSERT_UNIT_TEST(!str.compare("there and there and there"), "AString::overwrite", "0", iRet);

  str.assign("fob");
  str.overwrite(2, "obie blech", 4);
  ASSERT_UNIT_TEST(!str.compare("foobie"), "AString::overwrite", "1", iRet);

  str.assign("forple blech");
  str.overwrite(2, "obie");
  ASSERT_UNIT_TEST(!str.compare("foobie blech"), "AString::overwrite", "2", iRet);

  str.assign("sert");
  str.insert(0x0, "In");
  str.insert(AConstant::npos, "not here", 0x3, 0x5);
  ASSERT_UNIT_TEST(!str.compare("Insert here"), "AString::insert", "0", iRet);

  str.assign("Testing, of truncation, everything after 2nd comma will be removed!");
  str.truncateAt(',', 10);
  ASSERT_UNIT_TEST(!str.compare("Testing, of truncation"), "AString::truncateAt", "0", iRet);

  str.assign("Testing, of truncation, everything after 2nd comma will be removed!");
  str.truncateAtOneOf(AConstant::ASTRING_COMMA, 10);
  ASSERT_UNIT_TEST(!str.compare("Testing, of truncation"), "AString::truncateAtOneOf", "0", iRet);

  str.assign("Rest_and_test,,,,,");
  str.replace('R', 'T');
  str.replace('t', 'r', 6, 1);
  str.replace(';', '!');
  str.replace('_', ' ', 5);
  str.replace(',', '.', 0, 3);
  ASSERT_UNIT_TEST(!str.compare("Test_and rest...,,"), "AString::replace", "0", iRet);

  str.assign("mypath.ext/myfile.ext/somethingelse.ext");
  str.replace(".ext", ".data");
  ASSERT_UNIT_TEST(!str.compare("mypath.data/myfile.data/somethingelse.data"), "AString::replace", "1", iRet);

  str.replace(".data", ".x");
  ASSERT_UNIT_TEST(!str.compare("mypath.x/myfile.x/somethingelse.x"), "AString::replace", "2", iRet);

  str.replace(".x", "");
  ASSERT_UNIT_TEST(!str.compare("mypath/myfile/somethingelse"), "AString::replace", "3", iRet);

  str.replace("/", "\\");
  ASSERT_UNIT_TEST(!str.compare("mypath\\myfile\\somethingelse"), "AString::replace", "4", iRet);

  str.replace("notfound", "!!!");
  ASSERT_UNIT_TEST(!str.compare("mypath\\myfile\\somethingelse"), "AString::replace", "5", iRet);

  str.assign("fooTest1fooTest2fooYetfoo");
  str.replace("foo", "bar", 2, 2);
  ASSERT_UNIT_TEST(!str.compare("fooTest1barTest2barYetfoo"), "AString::replace", "6", iRet);

  str.replace("foo", "X", 0, 20);
  str.replace("bar", "X", 1, 20);
  ASSERT_UNIT_TEST(!str.compare("XTest1XTest2XYetX"), "AString::replace", "7", iRet);

  str.clear();
  str.appendN(4, 'q');
  ASSERT_UNIT_TEST(!str.compare("qqqq"), "AString::append", "0", iRet);

  str.clear();
  str.appendN(3, "foo");
  ASSERT_UNIT_TEST(!str.compare("foofoofoo"), "AString::append", "1", iRet);

  str.clear();
  str1.assign("test:");
  str.appendN(5, str1);
  ASSERT_UNIT_TEST(!str.compare("test:test:test:test:test:"), "AString::append", "2", iRet);

  return iRet;
}
