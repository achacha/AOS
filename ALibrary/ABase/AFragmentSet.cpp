/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AFragmentSet.hpp"

void AFragmentSet::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Set=" << m_Set << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Offset=" << m_Offset << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFragmentSet::AFragmentSet(AFragmentSet::SetType t)
{
	reset(); 
	switch(t) {
		case UppercaseAlpha :
			m_Set = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			break;

		case LowercaseAlpha :
			m_Set = "abcdefghijklmnopqrstuvwxyz";
			break;

		case Numeric :
			m_Set = "0123456789";
			break;

		case Alpha :
			m_Set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
			break;

		case AlphaNumeric :
			m_Set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
			break;

		case UppercaseAlphaNumeric :
			m_Set = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
			break;

		case LowercaseAlphaNumeric :
			m_Set = "abcdefghijklmnopqrstuvwxyz0123456789";
			break;

		default :
      ATHROW(this, AException::ProgrammingError);			// Invalid type
			break;
	}
}

AFragmentSet::AFragmentSet(const AString& strSet)
{ 
	reset(); 
	m_Set = strSet;
}

bool AFragmentSet::next()
{
	m_Offset++;
	if (m_Offset < m_Set.getSize()) {
		return true;
	}
	else {
		reset();
		return false;
	}
}

void AFragmentSet::emit(AOutputBuffer& target) const
{
	target.append(m_Set.at(m_Offset));
}

void AFragmentSet::reset()
{
	m_Offset = 0;
}

size_t AFragmentSet::getSetSize() const
{
	return m_Set.getSize();
}
