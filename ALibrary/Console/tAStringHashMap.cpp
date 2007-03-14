#include "AStringHashMap.hpp"
#include "ATextGenerator.hpp"
#include "AXmlElement.hpp"
#include "ARope.hpp"
#include "AFile_AString.hpp"

void testHashing()
{
  for (int i=0; i<32; ++i)
  {
    std::cout << (AString("item")+AString::fromInt(i)).getHash(13) << std::endl;
  }
}

void testHashmapSet()
{
  AStringHashMap shm(5);

  shm.set("item0", "data");
  shm.set("item1", "data");
  shm.set("item2", "data");
  shm.set("item3", "data");
  shm.set("item4", "data");
  shm.set("item5", "data");
  shm.set("item6", "data");
  shm.set("item7", "data");
  shm.set("item8", "data");
  shm.set("item9", "data");
  shm.set("item10", "data");
  shm.set("item11", "data");
  shm.set("item12", "data");
  shm.set("item13", "data");
  shm.set("item14", "data");
  shm.set("item15", "data");
  shm.set("item16", "data");
  shm.set("item17", "data");
  shm.set("item18", "data");
  shm.set("item19", "data");
  
  shm.debugDump();
}

void testHashmapSetRandom()
{
  AStringHashMap shm(5);
  AString str(32, 128);

  for(int i=0; i<100; ++i)
  {
    ATextGenerator::generateRandomWord(str, ARandomNumberGenerator::get().nextRange(12,2));
    shm.set(str, "0");
    str.clear();
  }

  shm.debugDump();
}

void testHashmapEmitXml()
{
  AXmlElement root("root");

  AStringHashMap shm(5);
  AString str(32, 128);

  for(int i=0; i<32; ++i)
  {
    ATextGenerator::generateRandomWord(str, ARandomNumberGenerator::get().nextRange(12,6));
    shm.set(str, "0");
    str.clear();
  }

  shm.emit(root);
  ARope rope;
  root.emitXml(rope, 0);
  std::cout << rope << std::endl;
}

void testHashmapEmit()
{
  AStringHashMap shm(5);
  AString str(32, 128);

  for(int i=0; i<32; ++i)
  {
    ATextGenerator::generateRandomWord(str, ARandomNumberGenerator::get().nextRange(12,6));
    shm.set(str, "0");
    str.clear();
  }

  ARope rope;
  shm.emit(rope);
  std::cout << rope << std::endl;
}

void testSerialize()
{
  AStringHashMap shm(5);
  AString str(32, 128);

  for(int i=0; i<4; ++i)
  {
    ATextGenerator::generateRandomWord(str, ARandomNumberGenerator::get().nextRange(12,6));
    shm.set(str, "0");
    str.clear();
  }

  std::cout << "Original emit" << std::endl;
  ARope rope;
  shm.emit(rope);
  std::cout << rope << std::endl;
  shm.debugDump();
  std::cout << std::endl;

  AFile_AString strfile(10240, 1024);
  shm.toAFile(strfile);
  AStringHashMap shmAfter(5);
  shmAfter.fromAFile(strfile);

  std::cout << "filedata" << std::endl;
  strfile.debugDump();
  std::cout << std::endl;

  std::cout << "After toAFile/fromAFile" << std::endl;
  rope.clear();
  shmAfter.emit(rope);
  std::cout << rope << std::endl;
  shmAfter.debugDump();
  std::cout << std::endl;
}

int main()
{
  //testHashing();
  //testHashmapSet();
  //testHashmapSetRandom();
  //testHashmapEmitXml();
  //testHashmapEmit();
  testSerialize();

  return 0;
}