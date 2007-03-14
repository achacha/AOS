#ifndef INCLUDED_ARng_Lecuyer_HPP_
#define INCLUDED_ARng_Lecuyer_HPP_

#include "apiABase.hpp"
#include "ARandomNumberGenerator.hpp"

#define RNG_LEBD_NTAB 32

/**
Native form: _nextUnit()
**/

class ABASE_API ARng_Lecuyer : public ARandomNumberGenerator
{
public:
  ARng_Lecuyer();

protected:
  virtual void   _initialize();            //a_Initilize the RNG
  virtual double _nextUnit();              //a_Get the next random double [0, 1.0)
  virtual u4     _nextU4();                //a_Get the next random u4     [0, ULONG_MAX)
  virtual u4     _nextRand();              //a_Get the next random u4     [0, RAND_MAX)

private:
  long m_lV[RNG_LEBD_NTAB];
  long m_lY;
  long m_lSeed;
  long m_lSeed2;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED_ARng_Lecuyer_HPP_

