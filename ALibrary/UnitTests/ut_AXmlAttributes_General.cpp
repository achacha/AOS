#include "pchUnitTests.hpp"
#include "AXmlAttributes.hpp"

int ut_AXmlAttributes_General()
{
  int iRet = 0;
  std::cerr << "ut_AAttributes_General" << std::endl;

  AXmlAttributes attrs;
  attrs.insert("foo", "1");
  attrs.insert("bar", "2.71");
  attrs.insert("bar", "3.14");
  attrs.insert("baz", "0");
  ASSERT_UNIT_TEST(attrs.size() == 3, "size", "0", iRet);

  //a_Test getNames()
  SET_AString names;
  attrs.getNames(names);
  ASSERT_UNIT_TEST(names.size() == 3, "getNames size", "0", iRet);
  ASSERT_UNIT_TEST(names.find("foo") != names.end(), "getNames find", "0", iRet);
  ASSERT_UNIT_TEST(names.find("bar") != names.end(), "getNames find", "1", iRet);
  ASSERT_UNIT_TEST(names.find("baz") != names.end(), "getNames find", "1", iRet);
  
  //a_Test emit
  AString str;
  attrs.emit(str);
  ASSERT_UNIT_TEST(str.equals("foo=\"1\" bar=\"3.14\" baz=\"0\""), "emit", "0", iRet);

  //a_Test more insert
  attrs.insert("foo", "11374");
  attrs.insert("gaz", "94041");
  attrs.insert("gaz", "94087");
  ASSERT_UNIT_TEST(attrs.size() == 4, "size", "0", iRet);
  
  //a_Emit after insert
  str.clear();
  attrs.emit(str);
  ASSERT_UNIT_TEST(str.equals("foo=\"11374\" bar=\"3.14\" baz=\"0\" gaz=\"94087\""), "emit", "1", iRet);

  //a_Remove
  attrs.remove("foo");
  attrs.remove("gaz");
  
  //a_Emit after remove
  str.clear();
  attrs.emit(str);
  ASSERT_UNIT_TEST(str.equals("bar=\"3.14\" baz=\"0\""), "emit", "2", iRet);

  //a_Test clear
  attrs.clear();
  ASSERT_UNIT_TEST(attrs.size() == 0, "clear", "0", iRet);

  return iRet;
}
