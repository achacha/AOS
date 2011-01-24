/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ARng_Marsaglia.hpp"
#include "ATime.hpp"

void ARng_Marsaglia::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "m_carry=" << m_carry << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_i=" << m_i << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Q={" << std::endl;
  for (int i=0; i<ARRAY_SIZE; ++i)
  {
    ADebugDumpable::indent(os, indent+2) << m_Q[i] << ",\t";
    if (i % 8 == 7)
      os << std::endl;
  }
  os << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ARng_Marsaglia::ARng_Marsaglia()
{
  _initialize();
}

void ARng_Marsaglia::_initialize()
{
  //a_One time init to a random seed based on tick count
  ::srand((unsigned int)ATime::getTickCount());

  m_i=1037;

  //a_Use rand() to seed the array and carry
  for (int i=0; i < ARRAY_SIZE; ++i)
    m_Q[i] = (u4(::rand()) << 0x10) + ::rand();

  m_carry = u4(((double)::rand() * double(61137367L / RAND_MAX)));
}

double ARng_Marsaglia::_nextUnit()
{
  double dRet = _nextU4();
  dRet /= (double)ULONG_MAX;
  return dRet;
}

u4 ARng_Marsaglia::_nextU4()
{ 
	u8 t, a=611373678LL;
	t = a * m_Q[m_i] + m_carry; 
	m_carry = u4(t>>32);
	
  if(--m_i)
	  return m_Q[m_i] = u4(t);
	
  m_i = 1037;
	return m_Q[0] = u4(t);
}

u4 ARng_Marsaglia::_nextRand()
{ 
  return u4(_nextUnit() * RAND_MAX);
}
