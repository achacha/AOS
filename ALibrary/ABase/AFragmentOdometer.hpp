#ifndef INCLUDED__AFragmentOdometer_HPP__
#define INCLUDED__AFragmentOdometer_HPP__

#include "apiABase.hpp"
#include "AFragmentInterface.hpp"
#include "ATextOdometer.hpp"

class AFragmentOdometer : public AFragmentInterface
{
public:
  /*!
  Counts from 000...000 to 999...999 
  */
	AFragmentOdometer(size_t digits = 6);
	virtual ~AFragmentOdometer() {}

	virtual bool next();
  virtual void emit(AOutputBuffer&) const;
	virtual void reset();
	virtual size_t getSetSize() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
	ATextOdometer m_Odometer;
  AString m_InitialValue;
};

#endif //INCLUDED__AFragmentOdometer_HPP__
