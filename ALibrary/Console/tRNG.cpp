#include <iostream>
#include "AString.hpp"
#include "ARandomNumberGenerator.hpp"
#include "ATime.hpp"
#include "ATextConverter.hpp"
#include "ATextGenerator.hpp"
#include "templateMapOfPtrs.hpp"
#include "ATimer.hpp"

void testUniform()
{
  int iX;
  ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform);
  for (iX = 0x0; iX < 20; ++iX)
    std::cout << rng.nextRand() << ", ";
  std::cout << std::endl;

  for (iX = 0x0; iX < 20; ++iX)
    std::cout << rng.nextRand() << ", ";
  std::cout << std::endl;

  double dR;
  for (iX = 0x0; iX < 80000000; ++iX)
  {  
    dR = rng.nextUnit();
    if (dR < 0.0 || dR >= 1.0)
    {
      //a_This is bad if U[0,1) returns something outside of the range
      std::cerr << "Out of range: unit(Uniform)=" << dR << std::endl;
      break;
    }
  }
}

void testRNGs()
{
  register int iX;
  const int DIST_COUNT = 3;  //a_Currently only have 3 distros
  double dArray[DIST_COUNT][256];
  for (iX = 0; iX < 256; ++iX)
  {
    dArray[0][iX] = 0.0;
    dArray[1][iX] = 0.0;
    dArray[2][iX] = 0.0;
  }

  const int iiTrials = 1000000;
  for (iX = 0x0; iX < iiTrials; ++iX)
  {
    dArray[0][ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform).nextU1()] += 1.0;
    dArray[1][ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).nextU1()] += 1.0;
    dArray[2][ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia).nextU1()] += 1.0;
  }

  //a_Average over trials
  for (iX = 0; iX < 256; ++iX)
  {

    std::cout << iX << ":\t" << dArray[0][iX];
    std::cout << "\t" <<  dArray[1][iX];
    std::cout << "\t" <<  dArray[2][iX] << std::endl;
  }
}

void testUID()
{
  AString str;
  for (int iX=0; iX < 1000; ++iX)
  {
    ATextGenerator::generateUniqueId(str);
    std::cout << str << std::endl;
  }
}

void testSimple()
{
  std::cout << "Uniform" << std::endl;
  for (int i=0; i<10; ++i)
    std::cout << ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform).nextUnit() << std:: endl;

  std::cout << "\nL'Ecuyer" << std::endl;
  for (int i=0; i<10; ++i)
    std::cout << ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).nextUnit() << std:: endl;

  std::cout << "\nMarsaglia" << std::endl;
  for (int i=0; i<10; ++i)
    std::cout << ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia).nextUnit() << std:: endl;
}

void testSimpleU1()
{
  std::cout << "Uniform" << std::endl;
  for (int i=0; i<10; ++i)
    std::cout << (int)ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform).nextU1() << std:: endl;

  std::cout << "\nL'Ecuyer" << std::endl;
  for (int i=0; i<10; ++i)
    std::cout << (int)ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).nextU1() << std:: endl;

  std::cout << "\nMarsaglia" << std::endl;
  for (int i=0; i<10; ++i)
    std::cout << (int)ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia).nextU1() << std:: endl;
}

void testRangeInt()
{
  int x;
  const u4 iiTrials = 400000L;
  
  ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform);
  for (u4 i=0; i<iiTrials; ++i)
  {
    x = rng.nextRange(96, 32);
    if (x < 32 || x >=96)
    {
      std::cout << "Error rng[32,96) = " << x << std:: endl;
    }
  }
  std::cout << "Uniform executed " << iiTrials << " trials." << std::endl;

  rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer);
  for (u4 i=0; i<iiTrials; ++i)
  {
    x = rng.nextRange(96, 32);
    if (x < 32 || x >=96)
    {
      std::cout << "Error rng[32,96) = " << x << std:: endl;
    }
  }
  std::cout << "L'Ecuyer executed " << iiTrials << " trials." << std::endl;

  rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia);
  for (u4 i=0; i<iiTrials; ++i)
  {
    x = rng.nextRange(96, 32);
    if (x < 32 || x >=96)
    {
      std::cout << "Error rng[32,96) = " << x << std:: endl;
    }
  }
  std::cout << "Marsaglia executed " << iiTrials << " trials." << std::endl;
}

void testDebugDump()
{
#ifdef __DEBUG_DUMP__
  int i;
  for (i=0;i<10;++i) ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform).nextUnit();
  for (i=0;i<10;++i) ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).nextUnit();
  for (i=0;i<10;++i) ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia).nextUnit();

  std::cout << "Uniform" << std::endl;
  ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform).debugDump();

  std::cout << "\nLecuyer" << std::endl;
  ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).debugDump();

  std::cout << "\nMarsaglia" << std::endl;
  ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia).debugDump();

#endif
}

void testTiming()
{
  ATimer timer;
  double dx;
  u4 ux;
  const u4 iiTrials = 10000000L;
  
  {
    ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform);
    timer.start();
    for (u4 i=0; i<iiTrials; ++i)
    {
      dx = rng.nextUnit();
    }
    timer.stop();
    std::cout << "Uniform unit executed " << iiTrials << " trials in " << timer.getInterval() << " msec" << std::endl;
  }

  {
    ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform);
    timer.start();
    for (u4 i=0; i<iiTrials; ++i)
    {
      dx = rng.nextU4();
    }
    timer.stop();
    std::cout << "Uniform rand executed " << iiTrials << " trials in " << timer.getInterval() << " msec" << std::endl;
  }

  {
    timer.clear();
    ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer);
    timer.start();
    for (u4 i=0; i<iiTrials; ++i)
    {
      dx = rng.nextUnit();
    }
    timer.stop();
    std::cout << "L'Ecuyer unit executed " << iiTrials << " trials in " << timer.getInterval() << " msec" << std::endl;
  }

  {
    timer.clear();
    ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer);
    timer.start();
    for (u4 i=0; i<iiTrials; ++i)
    {
      ux = rng.nextU4();
    }
    timer.stop();
    std::cout << "L'Ecuyer rand executed " << iiTrials << " trials in " << timer.getInterval() << " msec" << std::endl;
  }

  {
    timer.clear();
    ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia);
    timer.start();
    for (u4 i=0; i<iiTrials; ++i)
    {
      dx = rng.nextUnit();
    }
    timer.stop();
    std::cout << "Marsaglia unit executed " << iiTrials << " trials in " << timer.getInterval() << " msec" << std::endl;
  }

  {
    timer.clear();
    ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Marsaglia);
    timer.start();
    for (u4 i=0; i<iiTrials; ++i)
    {
      ux = rng.nextU4();
    }
    timer.stop();
    std::cout << "Marsaglia rand executed " << iiTrials << " trials in " << timer.getInterval() << " msec" << std::endl;
  }
}

void testSizes()
{
  ARandomNumberGenerator& rng = ARandomNumberGenerator::get();

  int i;
  std::cout << "U1" << std::endl;
  for (i=0;i<10;++i)
    std::cout << std::hex << (int)rng.nextU1() << std::endl;

  std::cout << "\nU2" << std::endl;
  for (i=0;i<10;++i)
    std::cout << std::hex << rng.nextU2() << std::endl;

  std::cout << "\nU4" << std::endl;
  for (i=0;i<10;++i)
    std::cout << std::hex << rng.nextU4() << std::endl;

  std::cout << "\nrand" << std::endl;
  for (i=0;i<10;++i)
    std::cout << std::hex << rng.nextRand() << std::endl;
}

void testDisplayUniformUnit()
{
  ARandomNumberGenerator& rng = ARandomNumberGenerator::get(ARandomNumberGenerator::Uniform);
  for (int i=0; i<20; ++i)
  {
    std::cout << rng.nextUnit() << std::endl;
  }
}

int main()
{
  //testUniform();
  //testRNGs();
  //testUID();
  //testSimple();
  //testSimpleU1();
  //testRangeInt();
  //testDebugDump();
  //testTiming();
  //testSizes();
  testDisplayUniformUnit();

  return 0x0;
}
