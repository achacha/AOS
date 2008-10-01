/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AException.hpp"
#include "ALock.hpp"
#include "ARng_Uniform.hpp"
#include "ARng_Lecuyer.hpp"
#include "ARng_Marsaglia.hpp"

//a_Static map
AMapOfPtrs<int, ARandomNumberGenerator> ARandomNumberGenerator::sm_rngMap;

class RandomNumberInitializer
{
public:
  RandomNumberInitializer()
  {
    ARandomNumberGenerator::init();
  }
};
static RandomNumberInitializer sg_InitializeRandomNumberGenerators;

void ARandomNumberGenerator::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "sm_rngMap.size=" << (u4)sm_rngMap.size() << std::endl;
  AMapOfPtrs<int, ARandomNumberGenerator>::const_iterator cit = sm_rngMap.begin();
  while (sm_rngMap.end() != cit)
  {
    ADebugDumpable::indent(os, indent+1) << "sm_rngMap[" << (*cit).first << "]=";
    (*cit).second->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void ARandomNumberGenerator::init()
{
  AASSERT(NULL, sm_rngMap.find(ARandomNumberGenerator::Uniform) == sm_rngMap.end());
  ARandomNumberGenerator::sm_rngMap[ARandomNumberGenerator::Uniform] = new ARng_Uniform();
  ARandomNumberGenerator::sm_rngMap[ARandomNumberGenerator::Lecuyer] = new ARng_Lecuyer();
  ARandomNumberGenerator::sm_rngMap[ARandomNumberGenerator::Marsaglia] = new ARng_Marsaglia();
}

ARandomNumberGenerator& ARandomNumberGenerator::get(RNG_TYPE rng)
{
  if (sm_rngMap.find(rng) == sm_rngMap.end())
    ATHROW(NULL, AException::InitializationFailure);

  return *(sm_rngMap[rng]);
}

double ARandomNumberGenerator::nextUnit()
{ 
  return _nextUnit();
}

u1 ARandomNumberGenerator::nextU1()
{
  return u1(_nextUnit() * 0x100);
}
  
u2 ARandomNumberGenerator::nextU2()
{
  return u2(_nextUnit() * 0x10000);
}

u8 ARandomNumberGenerator::nextU8()
{ 
  u8 ret = _nextU4();
  ret <<= 32;
  ret |= _nextU4();
  return ret;
}

u4 ARandomNumberGenerator::nextU4()
{ 
  return _nextU4();
}

u4 ARandomNumberGenerator::nextRand()
{ 
  return _nextRand();
}

int ARandomNumberGenerator::nextRange(int high, int low)
{
  return int(low + _nextUnit() * (high - low));
}

size_t ARandomNumberGenerator::nextRange(size_t high, size_t low)
{
  return size_t(low + _nextUnit() * (high - low));
}
