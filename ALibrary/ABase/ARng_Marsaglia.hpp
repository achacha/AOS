#ifndef INCLUDED_ARng_Marsaglia_HPP_
#define INCLUDED_ARng_Marsaglia_HPP_

#include "apiABase.hpp"
#include "ARandomNumberGenerator.hpp"

/**
Native form: _nextU4()
Period: 3056868392^33216-1
**/

class ABASE_API ARng_Marsaglia : public ARandomNumberGenerator
{
public:
  ARng_Marsaglia();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  virtual void   _initialize();            //a_Initilize the RNG
  virtual double _nextUnit();              //a_Get the next random double [0, 1.0)
  virtual u4     _nextU4();                //a_Get the next random u4     [0, ULONG_MAX)
  virtual u4     _nextRand();              //a_Get the next random u4     [0, RAND_MAX)

private:
  enum {
    ARRAY_SIZE = 1038
  };
  u4 m_carry;
	u4 m_i;
  u4 m_Q[ARRAY_SIZE];
};

#endif //INCLUDED_ARng_Marsaglia_HPP_
