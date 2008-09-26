/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AFragmentCounter_HPP__
#define INCLUDED__AFragmentCounter_HPP__

#include "apiABase.hpp"
#include "AFragmentInterface.hpp"

class AFragmentCounter : public AFragmentInterface
{
public:
	AFragmentCounter(u1 digits = 1, u4 stop_value = 9, u4 start_value = 0, s1 step = 1);
	virtual ~AFragmentCounter() {}

	virtual bool next();
  virtual void emit(AOutputBuffer&) const;
	virtual void reset();
	virtual size_t getSetSize() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
	AString m_strOdometer;
	u1      m_Digits;
	u4      m_StartValue;
	u4      m_Value;
	u4      m_StopValue;
	s1      m_Step;

	void _set();
};

#endif //INCLUDED__AFragmentCounter_HPP__
