/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AFragmentCounter.hpp"
#include <math.h>

void AFragmentCounter::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_strOdometer=" << m_strOdometer << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Digits=" << (int)m_Digits << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_StartValue=" << m_StartValue << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Value=" << m_Value << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_StopValue=" << m_StopValue << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Step=" << (int)m_Step << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFragmentCounter::AFragmentCounter(u1 digits, u4 stop_value, u4 start_value, s1 step)
{
	AASSERT(this, digits < 10);
	AASSERT(this, step);

	if (start_value > stop_value && step > 0)
		step = -step;
	if (start_value < stop_value && step < 0)
		step = -step;

	m_Digits = digits;
	m_StartValue = start_value;
	m_StopValue = stop_value;
	m_Step = step;

	reset();
}

bool AFragmentCounter::next()
{	
  if (m_Value == 0 && m_Step < 0) {
    reset();
    return false;
  }

  m_Value += m_Step;
	if (m_StopValue > m_StartValue) {
		if (m_Value > m_StopValue) {
			reset();
			return false;
		}
	}
	else {
		if (m_StopValue < m_StartValue) {
			if (m_Value < m_StopValue) {
				reset();
				return false;
			}
		}
	}

	__set();
	return true;
}

void AFragmentCounter::emit(AOutputBuffer& target) const
{
	target.append(m_strOdometer);
}

void AFragmentCounter::reset()
{
	m_Value = m_StartValue;

	m_strOdometer.clear();
	u1 digits = (u1)(log10((float)m_Value) + 1);
	u1 odosize = (digits > m_Digits ? digits : m_Digits);
	m_strOdometer.setSize(odosize);

	__set();
}

void AFragmentCounter::__set()
{
	u1 digits = (u1)(log10((float)m_Value) + 1);
	u1 odosize = (digits > m_Digits ? digits : m_Digits);
	m_strOdometer.fill('0');

	AString str;
	str.parseU4(m_Value);
	if (m_strOdometer.getSize() < str.getSize())
    m_strOdometer.setSize(str.getSize());

	str.reverse();
	memcpy(m_strOdometer.startUsingCharPtr(), str.c_str(), str.getSize());
	m_strOdometer.stopUsingCharPtr(odosize);
	m_strOdometer.reverse();
}

size_t AFragmentCounter::getSetSize() const
{ 
  size_t iS = max(m_StopValue, m_StartValue) - min(m_StopValue, m_StartValue) + 1;
	if (m_Step && (m_Step > 1 || m_Step < -1))
		iS = iS / abs(m_Step) + 1;

	return iS;
}
