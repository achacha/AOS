#include "pchABase.hpp"
#include "AFragmentOdometer.hpp"

#ifdef __DEBUG_DUMP__
void AFragmentOdometer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AFragmentOdometer @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Odometer={" << std::endl;
  m_Odometer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AFragmentOdometer::AFragmentOdometer(size_t digits /* = 6 */) :
  m_Odometer(digits)
{
  m_Odometer.emit(m_InitialValue);
}

void AFragmentOdometer::emit(AOutputBuffer& target) const
{
  m_Odometer.emit(target);
}

void AFragmentOdometer::reset()
{
  m_Odometer.clear();
}

size_t AFragmentOdometer::getSetSize() const
{
	size_t ret = 1;
  size_t subsetSize = m_Odometer.getSubset().getSize();
  for (size_t i = 0; i < m_Odometer.getSize(); ++i)
  {
    ret *= subsetSize;
  }

  return ret;
}

bool AFragmentOdometer::next()
{ 
  ++m_Odometer;
  if (m_InitialValue.equals(m_Odometer.get()))
    return false;
  else
    return true; 
}
