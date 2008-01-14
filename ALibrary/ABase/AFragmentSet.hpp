#ifndef INCLUDED__AFragmentSet_HPP__
#define INCLUDED__AFragmentSet_HPP__

#include "apiABase.hpp"
#include "AFragmentInterface.hpp"

class AFragmentSet : public AFragmentInterface
{
public:
	enum SetType {
		UppercaseAlpha,
		LowercaseAlpha,
		Numeric,
		Alpha,
		AlphaNumeric,
		UppercaseAlphaNumeric,
		LowercaseAlphaNumeric
	};

	AFragmentSet(SetType t);
	AFragmentSet(const AString& strSet);
	virtual ~AFragmentSet() {}

	virtual bool next();
	virtual void reset();
	virtual size_t getSetSize() const;
  virtual void emit(AOutputBuffer&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
	AFragmentSet() {}

	AString m_Set;
	size_t m_Offset;
};

#endif //INCLUDED__AFragmentDigit_HPP__
