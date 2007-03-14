#include "pchUnitTests.hpp"
#include "ABitArray.hpp"

int ut_ABitArray_General(void)
{
  int iRet = 0x0;

  std::cerr << "ut_ABitArray_General" << std::endl;

  ABitArray ar;
  AString original("01100101010101010001010101001010101010101"), result;
  ar.set(original, ABitArray::Binary);
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(original.equals(result), "Bit based copy not equal", "", iRet);

  result.clear();
  ar.all(1);
  original.fill('1');
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(original.equals(result), "Reset or AString::fill not behaving correctly", "", iRet);

  //a_Get/Set tests
  result.clear();
  ar.set("12345678", ABitArray::Hexadecimal);
  ar.get(result, ABitArray::Hexadecimal);
  ASSERT_UNIT_TEST(!result.compare("12345678"), "From and to hex not working", "", iRet);

  result.clear();
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("00010010001101000101011001111000"), "From hex to binary not working", "", iRet);

  result.clear();
  ar.set("00010010001101000101011001111000", ABitArray::Binary);
  ar.get(result, ABitArray::Hexadecimal);
  ASSERT_UNIT_TEST(!result.compare("12345678"), "From binary and to hex not working", "", iRet);

  result.clear();
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("00010010001101000101011001111000"), "From and to binary not working", "", iRet);

  result.clear();
  ar.setBit(0, 1);
  ar.setBit(2, 1);
  ar.get(result, ABitArray::Hexadecimal);
  ASSERT_UNIT_TEST(!result.compare("1234567d"), "Bit modify to hex", "", iRet);

  result.clear();
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("00010010001101000101011001111101"), "Bit modify to binary", "", iRet);

  result.clear();
  ar.set("cafe", ABitArray::Hexadecimal);
  ar.setBit(0,1);
  ar.get(result, ABitArray::Hexadecimal);
  u4 x = result.toU4(0x10);
  ASSERT_UNIT_TEST(x == 0xcaff, "Convert from hex to U4 failed", "", iRet);

  result.clear();
  ar.set("3");
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("0011"), "Convert from hex 1 char to binary", "", iRet);

  result.clear();
  ar.set("7F");
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("01111111"), "Convert from hex 2 char to binary", "0", iRet);

  result.clear();
  ar.set("7f");
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("01111111"), "Convert from hex 2 char to binary", "1", iRet);

  result.clear();
  ar.set("1FF");
  ar.get(result, ABitArray::Binary);
  ASSERT_UNIT_TEST(!result.compare("000111111111"), "Convert from hex 3 char to binary", "", iRet);

  result.clear();
  ar.set("111111111", ABitArray::Binary);
  ar.get(result, ABitArray::Hexadecimal);
  ASSERT_UNIT_TEST(!result.compare("01ff"), "Convert from binary to hex", "", iRet);

  return iRet;
}
