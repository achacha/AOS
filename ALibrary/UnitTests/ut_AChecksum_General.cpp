/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchUnitTests.hpp"
#include "AChecksum.hpp"
#include "AFile_AString.hpp"
#include "AString.hpp"

int ut_AChecksum_General()
{
  std::cerr << "ut_AChecksum_General" << std::endl;

  int iRet = 0x0;

  AString str;

  str.assign("0123456789ABCDEF");
  ASSERT_UNIT_TEST(0x983c37b5 == AChecksum::crc32(str), "AChecksum::crc32 failed checksum", "0", iRet);

  str.assign("A cyclic redundancy check (CRC) is a type of hash function used to produce a checksum, which is a small number of bits, from a large block of data, such as a packet of network traffic or a block of a computer file, in order to detect errors in transmission or storage.");
  ASSERT_UNIT_TEST(0x330af983 == AChecksum::crc32(str), "AChecksum::crc32 failed checksum", "1", iRet);

  str.assign("The CRC32 algorithm is a reliable way of checking whether an error has occurred in a sequence of bytes. There are various means of checking whether any bytes have been corrupted, for example, checksums and parity bits, but many simple schemes run a reasonably high chance of failing to detect an error depending on the type of corruption to the data that occurs. With CRC the chances of a corruption occurring which results in the same CRC are extremely remote.");
  ASSERT_UNIT_TEST(0x1df44297 == AChecksum::crc32(str), "AChecksum::crc32 failed checksum", "2", iRet);

  str.assign("0123456789ABCDEF");
  ASSERT_UNIT_TEST(0x1ccb03a3 == AChecksum::adler32(str), "AChecksum::adler32 failed checksum", "0", iRet);

  str.assign("An Adler-32 checksum is almost as reliable as a CRC-32 but can be computed much faster.");
  ASSERT_UNIT_TEST(0x002c1d81 == AChecksum::adler32(str), "AChecksum::adler32 failed checksum", "1", iRet);

  str.assign("Adler32 is similar to CRC-32, except that it is much faster and carries a slightly higher probability of collisions. The zlib compression library uses Adler32 as its checksum algorithm.");
  ASSERT_UNIT_TEST(0xa0694272 == AChecksum::adler32(str), "AChecksum::adler32 failed checksum", "2", iRet);

  return iRet;
}
