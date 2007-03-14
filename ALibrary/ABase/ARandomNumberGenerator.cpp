#include "pchABase.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AException.hpp"
#include "ALock.hpp"
#include "ARng_Uniform.hpp"
#include "ARng_Lecuyer.hpp"
#include "ARng_Marsaglia.hpp"

//a_Static map
AMapOfPtrs<int, ARandomNumberGenerator> ARandomNumberGenerator::sm_rngMap;

#ifdef __DEBUG_DUMP__
void ARandomNumberGenerator::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(ARandomNumberGenerator@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "sm_rngMap._map.size=" << (u4)sm_rngMap._map.size() << std::endl;
  AMapOfPtrs<int, ARandomNumberGenerator>::TYPEDEF::const_iterator cit = sm_rngMap._map.begin();
  while (sm_rngMap._map.end() != cit)
  {
    ADebugDumpable::indent(os, indent+1) << "sm_rngMap[" << (*cit).first << "]=";
    (*cit).second->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ARandomNumberGenerator& ARandomNumberGenerator::get(
  RNG_TYPE rng /* = ARandomNumberGenerator::Uniform */,
  ASynchronization *pSyncObject /* = NULL */
)
{
  if (sm_rngMap._map.find(rng) == sm_rngMap._map.end())
  {
    //a_Create (lock if not NULL)
    switch(rng)
    {
      case ARandomNumberGenerator::Uniform:
      {
        ALock lock(pSyncObject);
        sm_rngMap._map[ARandomNumberGenerator::Uniform] = new ARng_Uniform();
      }
      break;

      case ARandomNumberGenerator::Lecuyer:
      {
        ALock lock(pSyncObject);
        sm_rngMap._map[ARandomNumberGenerator::Lecuyer] = new ARng_Lecuyer();
      }
      break;

      case ARandomNumberGenerator::Marsaglia:
      {
        ALock lock(pSyncObject);
        sm_rngMap._map[ARandomNumberGenerator::Marsaglia] = new ARng_Marsaglia();
      }
      break;

      default:
        ATHROW(NULL, AException::NotImplemented);
    }
  }

  return *(sm_rngMap._map[rng]);
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
