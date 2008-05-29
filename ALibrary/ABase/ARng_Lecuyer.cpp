/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ARng_Lecuyer.hpp"
#include "ATime.hpp"

void ARng_Lecuyer::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(ARng_Lecuyer @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "m_lSeed=" << m_lSeed << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_lSeed2=" << m_lSeed2 << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_lY=" << m_lY << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_lV={" << std::endl;
  for (int i=0; i<RNG_LEBD_NTAB; ++i)
  {
    ADebugDumpable::indent(os, indent+2) << m_lV[i] << ",\t";
    if (7 == i % 8) os << std::endl;
  }
  os << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

//a_L'Ecuyer long period RNG with Bays-Durham shuffle
#define RNG_LEBD_IQ1  53668L
#define RNG_LEBD_IQ2  52774L
#define RNG_LEBD_IR1  12211L
#define RNG_LEBD_IR2  3791L
#define RNG_LEBD_IA1  40014L
#define RNG_LEBD_IA2  40692L
#define RNG_LEBD_IM1  2147483563L
#define RNG_LEBD_IM2  2147483399L
#define RNG_LEBD_IMM1 2147483562L         //a_RNG_LEBD_IM1 - 1
#define RNG_LEBD_AM0  0.4656613057392     //a_AM0 * AM1 = (1.0 / RNG_LEBD_IM1)
#define RNG_LEBD_AM1  1e-9 
#define RNG_LEBD_NDIV 67108861L           //a_(RNG_LEBD_IM1/RNG_LEBD_NTAB)
#define RNG_LEBD_EPS  2e-016              
#define RNG_LEBD_RNMX 0.999999999999999   //a_15 digit precision

//a_L'Ecuyer long-period random number generator
//a_Initialize ONCE with a negative seed and reuse the seed (hence the reference taken)
//a_Guarantees better results if the seed is initialized once and untouched and only modified by the routine
ARng_Lecuyer::ARng_Lecuyer() :
  m_lY(0x0L),
  m_lSeed2(123456789L)
{
  _initialize();
}

void ARng_Lecuyer::_initialize()
{
  m_lSeed = (s4)ATime::getTickCount();         //a_Random initialization seed
  m_lSeed2 = m_lSeed;

  //a_Let's get ready to shuffle!  (8 round warm-up)
  long lK;
  for (int iJ = RNG_LEBD_NTAB + 0x7; iJ >= 0x0; iJ--)
  {
    lK = m_lSeed / RNG_LEBD_IQ1;
    m_lSeed = RNG_LEBD_IA1 * (m_lSeed - lK * RNG_LEBD_IQ1) - lK * RNG_LEBD_IR1;
    if (m_lSeed < 0x0L)
      m_lSeed += RNG_LEBD_IM1;
    if (iJ < RNG_LEBD_NTAB)
      m_lV[iJ] = m_lSeed;
  }

  m_lY = m_lV[0x0];
}

double ARng_Lecuyer::_nextUnit()
{
  //a_Reinitialize if seed is <0
  if (m_lSeed <= 0 || !m_lY)
  {
    //a_Reset requested, generate a new seed!
    if (-m_lSeed < 0x1L)
      m_lSeed = 0x1L;
    else
      m_lSeed = -m_lSeed;

    _initialize();
  }

  //a_Start the generation of the random number
  long lK = m_lSeed / RNG_LEBD_IQ1;
  m_lSeed = RNG_LEBD_IA1 * (m_lSeed - lK * RNG_LEBD_IQ1) - lK * RNG_LEBD_IR1;
  if (m_lSeed < 0x0L) m_lSeed += RNG_LEBD_IM1;
  
  m_lSeed2 = RNG_LEBD_IA2 * (m_lSeed2 - lK * RNG_LEBD_IQ2) - lK * RNG_LEBD_IR2;
  if (m_lSeed < 0x0L) m_lSeed += RNG_LEBD_IM2;

  //a_Now we shuffle
  int iJ = m_lY / RNG_LEBD_NDIV;
  m_lY = m_lV[iJ] - m_lSeed2;
  m_lV[iJ] = m_lSeed;
  if (m_lY < 0x1L) m_lY += RNG_LEBD_IMM1;
  
  double dRet  = RNG_LEBD_AM0;
  dRet *= m_lY;
  dRet *= RNG_LEBD_AM1;

  //a_Do not return an endpoint, it isn't expected
  //a_If <0 then return 0, L'Ecuyer seems to have a rare case when it can be this may be due to an anomaly in the algorithm
  if (dRet > RNG_LEBD_RNMX)
    return RNG_LEBD_RNMX;
  if (dRet < 0.0)
    return 0.0;

  return dRet;
}

u4 ARng_Lecuyer::_nextU4()
{ 
  return (u4)(_nextUnit() * 0x100000000);
}

u4 ARng_Lecuyer::_nextRand()
{ 
  return (u4)(_nextUnit() * RAND_MAX);
}
