/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include <ADigest_SHA1.hpp>
#include "AFile_AString.hpp"
#include "AString.hpp"

int ut_ASHA1Digest_General()
{
  std::cerr << "ut_ASHA1Digest_General" << std::endl;

  int iRet = 0x0;
  ADigest_SHA1 digest;
  AString strData("abc");
  AFile_AString strfile(strData);
  AString strOut;
  digest.update(strData);
  digest.finalize();
  digest.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("a9993e364706816aba3e25717850c26c9cd0d89d"), "SHA1 failed checksum, string based", "0", iRet);

  //a_Test reset and file
  strOut.clear();
  digest.reset();
  digest.update(strfile);
  digest.finalize();
  digest.emit(strOut);
  ASSERT_UNIT_TEST(strOut.equals("a9993e364706816aba3e25717850c26c9cd0d89d"), "SHA1 failed checksum, file based", "1", iRet);

  // test 2
  strOut.clear();
  digest.reset();
  digest.update(ASWNL("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
  digest.finalize();
  digest.emit(strOut);
  //std::cout << strOut << std::endl;
  ASSERT_UNIT_TEST(strOut.equals("84983e441c3bd26ebaae4aa1f95129e5e54670f1"), "SHA1 failed checksum, file based", "2", iRet);

  return iRet;
}
