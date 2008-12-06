/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <ANameValuePair.hpp>

void testSimple(int& iRet)
{
  ANameValuePair nvPair("name", "alex");
  AString str;
  nvPair.emit(str);
  ASSERT_UNIT_TEST(str.equals("name=alex"), "Simple emit to AString failed", "", iRet);
  ASSERT_UNIT_TEST(nvPair.isName("name"), "Checking name", "", iRet);
  ASSERT_UNIT_TEST(nvPair.isValue("alex"), "Checking value", "", iRet);
  ASSERT_UNIT_TEST(!nvPair.isName("Name"), "Checking against wrong case", "", iRet);
  ASSERT_UNIT_TEST(nvPair.isNameNoCase("Name"), "Compare without case", "", iRet);
}

void testXml(int& iRet)
{
  //a_XML value parsing
  AString str;
  ANameValuePair nvPair(ANameValuePair::XML);
  size_t pos = 0;

  nvPair.parse("  \t \t   \n \r \r\n  iam   \r = \n \t  \"a'b'c'\" \r\n ", pos);
  nvPair.emit(str);
  ASSERT_UNIT_TEST(str == "iam=\"a'b'c'\"", "Handling of single quote in double quote error", "0", iRet);

  pos = 0;
  str.clear();
  nvPair.parse("  \t \t   \n \r \r\n  iam   \r = \t  \"Myvalue's value\"  ", pos);
  nvPair.emit(str);
  ASSERT_UNIT_TEST(str == "iam=\"Myvalue's value\"", "Handling of single quote in double quote error", "1", iRet);

  pos = 0;
  str.clear();
  nvPair.parse("iam = 'is \"quoted\" often'", pos);
  nvPair.emit(str);
  ASSERT_UNIT_TEST(str == "iam='is \"quoted\" often'", "Handling of double quote in single quote error", "", iRet);
}

void testRegValue(int& iRet)
{
  ANameValuePair nvPair;
  nvPair.setType(ANameValuePair::REGINI);
  size_t pos = 0;
  nvPair.parse(" \r\n \"coLoR\" =    \t \t  \"  blue  \t \" \r\n  ", pos);

  ASSERT_UNIT_TEST(nvPair.isName("coLoR"), "RegIni name test", "", iRet);
  ASSERT_UNIT_TEST(nvPair.isValue("  blue  \t "), "RegIni value test", "", iRet);
}

void testHtml(int& iRet)
{
  ANameValuePair nvPair;
  nvPair.setType(ANameValuePair::HTML);
  size_t pos = 0;
  nvPair.parse(" \r\n coLoR =    \t \t  \" rEd \" \r\n  ", pos);

  ASSERT_UNIT_TEST(nvPair.isName("color"), "Html name test", "", iRet);
  ASSERT_UNIT_TEST(nvPair.isValue(" rEd "), "Html value test", "", iRet);
}

void testJson(int& iRet)
{
  ANameValuePair nvPair(ANameValuePair::JSON);
  size_t pos = 0;
  nvPair.parse(" \r\n \t coLoR :    \t \t  \" green's  yellow \" \r\n} \t ", pos);

  ASSERT_UNIT_TEST(nvPair.isName("coLoR"), "Json name test", "", iRet);
  ASSERT_UNIT_TEST(nvPair.isValue(" green's  yellow "), "Json value test", "", iRet);

  AString str('{');
  nvPair.emit(str);
  str.append('}');
  ASSERT_UNIT_TEST(str.equals("{coLoR:\" green's  yellow \"}"), "Json emit test", "", iRet);
}

void testFormMultiPart(int& iRet)
{
  AString str(" form-data; name=\"file\"; filename=\"TODO.txt\"");
  ANameValuePair nv(ANameValuePair::FORM_MULTIPART);
  size_t i = 0;
  nv.parse(str, i);
  ASSERT_UNIT_TEST(nv.getName().equals("form-data"), "FormMultiPart parse", "0n", iRet);
  ASSERT_UNIT_TEST(nv.getValue().isEmpty(), "FormMultiPart parse", "0v", iRet);
  
  nv.parse(str, i);
  ASSERT_UNIT_TEST(nv.getName().equals("name"), "FormMultiPart parse", "1n", iRet);
  ASSERT_UNIT_TEST(nv.getValue().equals("file"), "FormMultiPart parse", "1v", iRet);

  nv.parse(str, i);
  ASSERT_UNIT_TEST(nv.getName().equals("filename"), "FormMultiPart parse", "1n", iRet);
  ASSERT_UNIT_TEST(nv.getValue().equals("TODO.txt"), "FormMultiPart parse", "1v", iRet);
}

int ut_ANameValuePair_General()
{
  int iRet = 0x0;

  std::cerr << "ut_ANameValuePair_General" << std::endl;
  
  testSimple(iRet);
  NEWLINE_UNIT_TEST();
  testXml(iRet);
  NEWLINE_UNIT_TEST();
  testJson(iRet);
  NEWLINE_UNIT_TEST();
  testRegValue(iRet);
  NEWLINE_UNIT_TEST();
  testHtml(iRet);
  NEWLINE_UNIT_TEST();
  testFormMultiPart(iRet);

  return iRet;
}
