/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchUnitTests.hpp"
#include "AStringHashMap.hpp"
#include "ATextGenerator.hpp"
#include "AXmlElement.hpp"
#include "ARope.hpp"
#include "AFile_AString.hpp"

void testHashMapSerialize(int& iRet)
{
  for (int x=0; x<10; ++x)
  {
    AStringHashMap shm(NULL, 5);
    AString str(32, 128);

    for(int i=0; i<ARandomNumberGenerator::get().nextRange(32,6); ++i)
    {
      ATextGenerator::generateRandomWord(str, ARandomNumberGenerator::get().nextRange(12,6));
      shm.set(str, "0");
      str.clear();
    }

    AStringHashMap shmAfter(NULL, 5);
    AFile_AString strfile(10240, 1024);
    shm.toAFile(strfile);
    shmAfter.fromAFile(strfile);

    AString strBefore(2048, 256), strAfter(2048, 256);
    shm.emit(strBefore);
    shmAfter.emit(strAfter);

    if (strBefore != strAfter)
    {
      //a_Dump if not matching for debugging
      std::cout << strBefore << std::endl;
      shm.debugDump();
      std::cout << strAfter << std::endl;
      shmAfter.debugDump();
    }

    ASSERT_UNIT_TEST(strBefore == strAfter, "serialize", "", iRet);
  }
}

void testHashMapGetSet(int& iRet)
{
  AStringHashMap shm;

  //a_Set
  shm.set("item_000", "value0");
  shm.set("item_001", "value1");
  shm.set("item_002", "value2");
  shm.set("item_003", "value3");
  shm.set("item_004", "value4");
  ASSERT_UNIT_TEST(shm.getSize() == 5, "getSize", "0", iRet);
  shm.set("1_item_000", "value10");
  shm.set("2_item_001", "value11");
  shm.set("3_item_002", "value12");
  shm.set("4_item_003", "value13");
  shm.set("5_item_004", "value14");
  ASSERT_UNIT_TEST(shm.getSize() == 10, "getSize", "1", iRet);

  //a_Remove
  shm.remove("item_002");
  shm.remove("item_004");
  shm.remove("2_item_001");
  shm.remove("4_item_003");
  ASSERT_UNIT_TEST(shm.getSize() == 6, "getSize", "2", iRet);

  //a_Get
  AString str;
  ASSERT_UNIT_TEST(shm.get("item_000", str), "get", "0", iRet);
  ASSERT_UNIT_TEST(str == "value0", "get", "1", iRet);
  str.clear();
  ASSERT_UNIT_TEST(shm.get("5_item_004", str), "get", "2", iRet);
  ASSERT_UNIT_TEST(str == "value14", "get", "3", iRet);
  ASSERT_UNIT_TEST(!shm.get("item_002", str), "get", "4", iRet);
  ASSERT_UNIT_TEST(!shm.get("2_item_001", str), "get", "5", iRet);

  //a_Exists
  shm.set("item_005", "value5");
  shm.set("item_006", "value6");
  ASSERT_UNIT_TEST(!shm.exists("4_item_003"), "exists", "0", iRet);
  ASSERT_UNIT_TEST(!shm.exists("item_004"), "exists", "1", iRet);
  ASSERT_UNIT_TEST(shm.exists("item_006"), "exists", "2", iRet);

  //a_More get
  shm.set("item_000", "new_value");
  str.clear();
  ASSERT_UNIT_TEST(shm.get("item_000", str), "get", "6", iRet);
  ASSERT_UNIT_TEST(str == "new_value", "get", "7", iRet);

  shm.clear();
  ASSERT_UNIT_TEST(shm.getSize() == 0, "getSize", "3", iRet);
}

int ut_AStringHashMap_General()
{
  std::cerr << "ut_AStringHashMap_General" << std::endl;

  int iRet = 0x0;
  
  testHashMapSerialize(iRet);
  std::cerr << std::endl;
  testHashMapGetSet(iRet);

  return iRet;
}
