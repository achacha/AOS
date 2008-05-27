/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED_ARandomNumberGenerator_HPP_
#define INCLUDED_ARandomNumberGenerator_HPP_

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ASynchronization.hpp"
#include "templateMapOfPtrs.hpp"

/*!
Wrapper to all Random Number Generator (RNG) classes and base class

If this is to be used in a multithreaded environment you can pass ASynchronization object to serialize the first call
or during initialization call .get() for the RNG to be used so it is created once and saved

Usage:
u4 myulongrand = ARandomNumberGenerator::get().rand();

or

double mydoublerand = ARandomNumberGenerator::get(ARandomNumberGenerator::Lecuyer).unit();

or

ARandomNumberGenerator& rng = ARandomNumberGenerator::get();
u4 random_sum = 0;
for (int i=0; i<10; ++i)
  random_sum += rng.randU1();



Relative tests (YMMV):
  Uniform   unit executed 10000000 trials in 2657 msec
  Uniform   rand executed 10000000 trials in 2640 msec
  L'Ecuyer  unit executed 10000000 trials in 2782 msec     <- best double rand()
  L'Ecuyer  rand executed 10000000 trials in 3672 msec
  Marsaglia unit executed 10000000 trials in 2813 msec
  Marsaglia rand executed 10000000 trials in 1687 msec     <- best integer rand*()
*/

class ABASE_API ARandomNumberGenerator : public ADebugDumpable
{
public:
  enum RNG_TYPE {
    /*!
    Uniform distribution
    Integer based
    Implements srand() and rand()
    Natively returns u4 type
    */
    Uniform = 0,
    
    /*!
    Long period RNG (based on work by Pierre L'Ecuyer - http://www.iro.umontreal.ca/~LEcuyer/)
    Uses Bays-Durham shuffle
    Floating point based
    Best used with calls to unit() since results are based on [0,1)
    */
    Lecuyer = 1,

    /*!
    DEFAULT
    Dr. George Marsaglia's RNG
    Integer based, faster than srand/rand
    Period is estimated to be 3056868392^33216-1
    Best used with calls to rand* and rangeInt since it is based on [0, ULONG_MAX)
    */
    Marsaglia = 2
  };

  /*!
  Returns an instance of the RNG, creates it if needed, stores the created RNGs in memory
  Not synchronized, will try to use a sync object if you pass it one to synchronize creation (e.g. mutex, cs, etc)
  */
  static ARandomNumberGenerator& get(RNG_TYPE rng = ARandomNumberGenerator::Marsaglia, ASynchronization *pSyncObject = NULL);
    
  /*!
  Interface wrapper functions that use _next()
  */
  inline double nextUnit();                           //a_[0,1)
  inline u4     nextRand();                           //a_[0, RAND_MAX)  (usually 32767 or 0x7fff)
  inline u8     nextU8();                             //a_[0, 0x10000000000000000)
  inline u4     nextU4();                             //a_[0, 0x100000000)
  inline u2     nextU2();                             //a_[0, 0x10000)
  inline u1     nextU1();                             //a_[0, 256)

  /*!
  Ranged by type [low, high)
  To get random letters from A-Z inclusive use nextRange('Z'+1, 'A') or use ATextGenerator class
  
    nextRange(100,0)    - all whole numbers 0 to 99
    nextRange(20,10)    - all whole numbers 10 to 19
    nextRange('f','a')  - 'a' 'b' 'c' 'd' 'e'
  */
  inline int nextRange(int high, int low = 0x0);
  inline size_t nextRange(size_t high, size_t low = 0x0);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  RNG specific, each RNG specifies these, rest is handled by wrappers
  */
  virtual void   _initialize() = 0;            //a_Initilize the RNG
  virtual double _nextUnit() = 0;              //a_Get the next random double [0, 1.0)
  virtual u4     _nextU4()   = 0;              //a_Get the next random u4     [0, ULONG_MAX)
  virtual u4     _nextRand() = 0;              //a_Get the next random u4     [0, RAND_MAX)

private:
  static AMapOfPtrs<int, ARandomNumberGenerator> sm_rngMap;
};

#endif //INCLUDED_ARandomNumberGenerator_HPP_
