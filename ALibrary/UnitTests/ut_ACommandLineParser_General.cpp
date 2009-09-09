/*
Written by Alex Chachanashvili

$Id: ut_AChecksum_General.cpp 205 2008-05-29 13:58:16Z achacha $
*/
#include "pchUnitTests.hpp"
#include "ACommandLineParser.hpp"

int ut_ACommandLineParser_General()
{
  std::cerr << "ut_ACommandLineParser_General" << std::endl;

  int iRet = 0x0;

  const int argc = 7;
  char *argv[argc] = {
    "/foo/bar/this/unittest",
    "-help",
    "-f",
    "abc.txt",
    "-t",
    "100",
    "dosomething"
  };

  ACommandLineParser cmdLine(argc, argv);
  ASSERT_UNIT_TEST(cmdLine.existsAnonValue(ASWNL("dosomething")), "ACommandLineParser::existsAnonValue", "0", iRet);
  ASSERT_UNIT_TEST(cmdLine.existsAnonValue(ASWNL("dosomething")), "ACommandLineParser::existsAnonValue", "1", iRet);
  
  ASSERT_UNIT_TEST(cmdLine.existsNamedValue(ASWNL("f")), "ACommandLineParser::existsNamedValue", "0", iRet);
  ASSERT_UNIT_TEST(cmdLine.existsNamedValue(ASWNL("help")), "ACommandLineParser::existsNamedValue", "1", iRet);
  ASSERT_UNIT_TEST(cmdLine.existsNamedValue(ASWNL("t")), "ACommandLineParser::existsNamedValue", "2", iRet);

  AString value;
  cmdLine.getNamedValue(ASWNL("t"), value);
  ASSERT_UNIT_TEST(value.equals("100"), "ACommandLineParser::getNamedValue", "0", iRet);

  value.clear();
  cmdLine.getNamedValue(ASWNL("help"), value);
  ASSERT_UNIT_TEST(value.isEmpty(), "ACommandLineParser::getNamedValue", "1", iRet);

  value.clear();
  cmdLine.getNamedValue(ASWNL("f"), value);
  ASSERT_UNIT_TEST(value.equals("abc.txt"), "ACommandLineParser::getNamedValue", "1", iRet);

  value.clear();
  cmdLine.emit(value);
  ASSERT_UNIT_TEST(value.equals("/foo/bar/this/unittest -f abc.txt -help -t 100 dosomething"), "ACommandLineParser::emit", "0", iRet);

  value.clear();
  cmdLine.emitFilename(value);
  ASSERT_UNIT_TEST(value.equals("/foo/bar/this/unittest"), "ACommandLineParser::emitFilename", "0", iRet);

  value.clear();
  cmdLine.emitNamedParamaters(value);
  ASSERT_UNIT_TEST(value.equals("-f abc.txt -help -t 100"), "ACommandLineParser::emitNamedParamaters", "0", iRet);

  value.clear();
  cmdLine.emitAnonValues(value);
  ASSERT_UNIT_TEST(value.equals("dosomething"), "ACommandLineParser::emitAnonValues", "0", iRet);

  return iRet;
}
