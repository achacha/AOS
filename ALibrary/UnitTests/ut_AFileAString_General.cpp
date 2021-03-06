/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include <AFile_AString.hpp>

int ut_AFileAString_General()
{
  std::cerr << "ut_AFile_AString_General" << std::endl;

  int iRet = 0x0;

  AString str;
  AFile_AString strfile("This is a test\nLine2\n");

  str.clear();
  strfile.readLine(str);
  ASSERT_UNIT_TEST(!str.compare("This is a test"), "AFile_AString::readLine", "0", iRet);

  str.clear();
  strfile.write(AString("Line3"));
  strfile.write(AConstant::ASTRING_LF);
  ASSERT_UNIT_TEST(!strfile.useAString().compare("This is a test\nLine2\nLine3\n"), "AFile_AString::write", "0", iRet);

  str.clear();
  strfile.readLine(str);
  ASSERT_UNIT_TEST(!str.compare("Line2"), "AFile_AString::readLine", "1", iRet);

  str.clear();
  strfile.readLine(str);
  ASSERT_UNIT_TEST(!str.compare("Line3"), "AFile_AString::readLine", "2", iRet);

  str.clear();
  u4 ret = strfile.readLine(str);
  ASSERT_UNIT_TEST(ret == AConstant::npos, "AFile_AString::readLine", "3", iRet);

  str.clear();
  strfile.useAString().clear();
  strfile.reset();
  strfile.write(ASWNL("Test\n"));
  strfile.setWritePos(4);
  strfile.writeLine(ASWNL("Line 000"));
  strfile.setReadPos(4);
  strfile.readUntilOneOf(str);
  ASSERT_UNIT_TEST(!str.compare("Line"), "AFile_AString::readUntilOneOf", "0", iRet);

  str.clear();
  strfile.readUntilOneOf(str);
  ASSERT_UNIT_TEST(!str.compare("000"), "AFile_AString::readUntilOneOf", "1", iRet);

  str.clear();
  strfile.readUntilEOF(str);
  str.stripTrailing();  //a_Remove white space
  ASSERT_UNIT_TEST(str.isEmpty(), "AFile_AString::readUntilEOF", "0", iRet);

  {
    AFile_AString source("0123456789");
    AFile_AString target;

    ASSERT_UNIT_TEST(4 == target.write(source, 4), "AFile_AString::write", "0", iRet);
    ASSERT_UNIT_TEST(2 == target.write(source, 2), "AFile_AString::write", "1", iRet);
    ASSERT_UNIT_TEST(4 == target.write(source, 5), "AFile_AString::write", "2", iRet);
    ASSERT_UNIT_TEST(0 == target.write(source, 1), "AFile_AString::write", "3", iRet);
    ASSERT_UNIT_TEST(target.useAString().equals("0123456789"), "AFile_AString::write", "4", iRet);
  }
  

  return iRet;
}
