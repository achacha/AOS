#include <iostream>
#include <AXmlDocument.hpp>
#include <AFile_IOStream.hpp>
#include <ATimer.hpp>
#include <ARandomNumberGenerator.hpp>
#include <AFile_Physical.hpp>

void testCreate()
{
  AXmlDocument obj("object");
  

  std::cout << obj << std::endl;

}

//void testSpeed()
//{
//  AObject obj;
//  int iX, iTestSize = 1024;
//  ATimer timerX;
//
//  timerX.start();
//  for (iX = 0x0; iX < iTestSize; ++iX)
//    obj.set(AString::fromInt(iX), ARandomNumberGenerator::getRandomString(6));
//  timerX.stop();
//
//  cerr << "Elapsed time (ms) for " << iX << " insertions = " << timerX.getInterval() << endl;
//
//  timerX.start();
//  for (iX = 0x0; iX < iTestSize; ++iX)
//    obj.exists(ARandomNumberGenerator::getRandomString(6));
//  timerX.stop();
//
//  cerr << "Elapsed time (ms) for " << iX << " searches   = " << timerX.getInterval() << endl;
//
//  timerX.start();
//  for (iX = 0x0; iX < iTestSize; ++iX)
//    obj.remove(AString::fromInt(iX));
//  timerX.stop();
//
//  cerr << "Elapsed time (ms) for " << iX << " removals   = " << timerX.getInterval() << endl;
//
//  cerr << "Final size = " << obj.getSize() << " should be 0" << endl;
//}
//
//void testDebugDump()
//{
//  AObject obj;
//  for (int iX = 0x0; iX < 16; ++iX)
//    obj.set(AString::fromInt(iX), ARandomNumberGenerator::getRandomString(6));
//
//  cerr << obj.toString() << endl;
//  obj.debugDump(cerr, 0x0);
//}
//
//void testSerialization()
//{
//  AObject obj0, obj1;
//  for (int iX = 0x0; iX < 16; ++iX)
//    obj0.set(ARandomNumberGenerator::getRandomString(6), ARandomNumberGenerator::getRandomString(6));
//
//  cerr << "--BEFORE--" << endl;
//  obj0.debugDump(cerr, 0x0);
//  {
//    AFilePhysical file;
//    file.open("obj.cgw", AFilePhysical::smstr_Write);
//    obj0.toAFile(file);
//    file.close();
//  }
//
//
//  {
//    AFilePhysical file;
//    file.open("obj.cgw", AFilePhysical::smstr_ReadOnly);
//    obj1.fromAFile(file);
//    file.close();
//  }
//  obj1.debugDump(cerr, 0x0);
//
//  if (obj1.compare(obj0))
//    cerr << endl << "Objects are NOT equal!" << endl;
//  else
//    cerr << endl << "Objects are equal" << endl;
//
//}

int main()
{
  testCreate();
  //testSpeed();
  //testDebugDump();
  //testSerialization();


  return 0x0;
}
