/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ARng_Uniform.hpp"
#include "ATime.hpp"

void ARng_Uniform::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(ARng_Uniform @ " << std::hex << this << std::dec << ");" << std::endl;
}

ARng_Uniform::ARng_Uniform()
{
  _initialize();
}

void ARng_Uniform::_initialize()
{
  AASSERT(this, RAND_MAX == 0x7fff);  //a_If this changed from the ANSI definition, adjust _nextU4() accordingly to give [0, ULONG_MAX) result

  //a_One time init to a random seed based on tick count
  ::srand((unsigned int)ATime::getTickCount());
}

double ARng_Uniform::_nextUnit()
{
  return ((double)_nextU4()) / (double(ULONG_MAX) + 1.0);   //a_[0, 1.0)  add 1 to remove to 1.0
}

u4 ARng_Uniform::_nextU4()
{
  u4 uA = (u4(::rand()) & 0x00000fff) << 0x14;
  u4 uB = uA + ((u4(::rand()) & 0x00000fff) << 0x8);
  u4 uC = uB + (u4(::rand()) & 0x000000ff);
  return uC;
}

u4 ARng_Uniform::_nextRand()
{
  return u4(::rand());
}
