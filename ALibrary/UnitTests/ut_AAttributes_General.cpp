#include "pchUnitTests.hpp"
#include "AAttributes.hpp"

int ut_AAttributes_General()
{
  int iRet = 0;
  std::cerr << "ut_AAttributes_General" << std::endl;

  AAttributes attrs;
  attrs.insert("foo", "1");
  attrs.insert("bar", "2.71");
  attrs.insert("bar", "3.14");
  attrs.insert("bar", "13");
  attrs.insert("bar", "42");
  attrs.insert("baz", "0");
  ASSERT_UNIT_TEST(attrs.size() == 6, "size", "", iRet);

  //a_Test getNames()
  SET_AString names;
  attrs.getNames(names);
  ASSERT_UNIT_TEST(names.size() == 3, "getNames count", "", iRet);
  ASSERT_UNIT_TEST(names.find("foo") != names.end(), "getNames find", "0", iRet);
  ASSERT_UNIT_TEST(names.find("bar") != names.end(), "getNames find", "1", iRet);
  ASSERT_UNIT_TEST(names.find("baz") != names.end(), "getNames find", "1", iRet);

  //a_Test get
  LIST_AString pairs;
  attrs.get("bar", pairs);
  ASSERT_UNIT_TEST(pairs.size() == 4, "get count", "", iRet);
  
  //a_Test getDilimited
  AString str;
  attrs.getDelimited("bar", str, ":");
  ASSERT_UNIT_TEST(str.find("2.71") != AConstant::npos, "getDelimited", "0", iRet);
  ASSERT_UNIT_TEST(str.find("3.14") != AConstant::npos, "getDelimited", "1", iRet);
  ASSERT_UNIT_TEST(str.find("13") != AConstant::npos, "getDelimited", "2", iRet);
  ASSERT_UNIT_TEST(str.find("42") != AConstant::npos, "getDelimited", "3", iRet);

  //a_Test count
  ASSERT_UNIT_TEST(attrs.count("foo") == 1, "count", "0", iRet);
  ASSERT_UNIT_TEST(attrs.count("bar") == 4, "count", "1", iRet);
  ASSERT_UNIT_TEST(attrs.count("baz") == 1, "count", "2", iRet);

  //a_Test remove
  attrs.insert("gaz", "11374");
  attrs.insert("gaz", "94041");
  attrs.insert("gaz", "94087");
  attrs.remove("gaz", "11374");
  ASSERT_UNIT_TEST(attrs.size() == 8, "remove size", "0", iRet);

  pairs.clear();
  attrs.get("gaz", pairs);
  ASSERT_UNIT_TEST(pairs.size() == 2, "remove size", "1", iRet);

  attrs.remove("gaz");
  pairs.clear();
  attrs.get("gaz", pairs);
  ASSERT_UNIT_TEST(pairs.size() == 0, "remove size", "2", iRet);
  
  attrs.remove("bar");
  ASSERT_UNIT_TEST(attrs.size() == 2, "remove size", "3", iRet);
  
  //a_Test clear
  attrs.clear();
  ASSERT_UNIT_TEST(attrs.size() == 0, "clear", "", iRet);

  return iRet;
}
