/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AFragmentConstant.hpp"

void AFragmentConstant::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AFragmentConstant @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Constant=" << m_Constant << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFragmentConstant::AFragmentConstant(const AString& str)
{
  m_Constant = str;
}

void AFragmentConstant::emit(AOutputBuffer& target) const
{
	target.append(m_Constant);
}

void AFragmentConstant::reset()
{
}

size_t AFragmentConstant::getSetSize() const
{
	// Monolithic item
	return 1;
}

bool AFragmentConstant::next()
{ 
  return false; 
}
