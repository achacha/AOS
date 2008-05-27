/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED_ARng_Uniform_HPP_
#define INCLUDED_ARng_Uniform_HPP_

#include "apiABase.hpp"
#include "ARandomNumberGenerator.hpp"

/**
Native form: _nextU4()
**/
class ABASE_API ARng_Uniform : public ARandomNumberGenerator
{
public:
  ARng_Uniform();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  virtual void   _initialize();            //a_Initilize the RNG
  virtual double _nextUnit();              //a_Get the next random double [0, 1.0)
  virtual u4     _nextU4();                //a_Get the next random u4     [0, ULONG_MAX)
  virtual u4     _nextRand();              //a_Get the next random u4     [0, RAND_MAX)
};

#endif //INCLUDED_ARng_Uniform_HPP_
