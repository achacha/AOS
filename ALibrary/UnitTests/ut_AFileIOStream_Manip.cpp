/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include <AFile_IOStream.hpp>
#include <sstream>

int ut_AFileIOStream_Manip()
{
  std::cerr << "ut_AFileIOStream_Manip" << std::endl;

  int iRet = 0x0;

  {
    std::stringstream stream0;
		stream0 << "Line 0\rLine 1\nLine 2\r\nLine 3\n\rLine 4\nLine5";
    AFile_IOStream fileStream(stream0, stream0);
    AString str;

    ASSERT_UNIT_TEST(fileStream.isNotEof(), "AFile_IOStream::isNotEof", "0", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readLine(str), "AFile_IOStream::readLine", "0", iRet);
    ASSERT_UNIT_TEST(str == "Line 0\rLine 1", "AFile_IOStream::readLine content", "0", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readLine(str), "AFile_IOStream::readLine", "1", iRet);
    ASSERT_UNIT_TEST(str == "Line 2", "AFile_IOStream::readLine content", "1", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readLine(str), "AFile_IOStream::readLine", "2", iRet);
    ASSERT_UNIT_TEST(str == "Line 3", "AFile_IOStream::readLine content", "2", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readLine(str), "AFile_IOStream::readLine", "3", iRet);
    ASSERT_UNIT_TEST(str == "\rLine 4", "AFile_IOStream::readLine content", "3", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readLine(str), "AFile_IOStream::readLine", "4", iRet);
    ASSERT_UNIT_TEST(str == "Line5", "AFile_IOStream::readLine content", "4", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos == fileStream.readLine(str), "AFile_IOStream::readLine", "5", iRet);
  }

  {
    std::stringstream stream0;
    stream0 << "Word0 Word1\tWord2\rWord3\n\t \r";
    AFile_IOStream fileStream(stream0, stream0);
    AString str;
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "0", iRet);
    ASSERT_UNIT_TEST(str == "Word0", "AFile_IOStream::readUntilOneOf content", "0", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "1", iRet);
    ASSERT_UNIT_TEST(str == "Word1", "AFile_IOStream::readUntilOneOf content", "1", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "2", iRet);
    ASSERT_UNIT_TEST(str == "Word2", "AFile_IOStream::readUntilOneOf content", "2", iRet);

    str.clear();
    ASSERT_UNIT_TEST(AConstant::npos != fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "3", iRet);
    ASSERT_UNIT_TEST(str == "Word3", "AFile_IOStream::readUntilOneOf content", "3", iRet);

    str.clear();
    ASSERT_UNIT_TEST(0 == fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "4", iRet);
    ASSERT_UNIT_TEST(0 == fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "5", iRet);
    ASSERT_UNIT_TEST(0 == fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "6", iRet);
    ASSERT_UNIT_TEST(AConstant::npos == fileStream.readUntilOneOf(str), "AFile_IOStream::readUntilOneOf", "7", iRet);
  }

  return iRet;
}
