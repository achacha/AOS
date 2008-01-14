#ifndef INCLUDED__AFragmentConstant_HPP__
#define INCLUDED__AFragmentConstant_HPP__

#include "apiABase.hpp"
#include "AFragmentInterface.hpp"

class AFragmentConstant : public AFragmentInterface
{
public:
	AFragmentConstant(const AString& str);
	virtual ~AFragmentConstant() {}

	virtual bool next();
  virtual void emit(AOutputBuffer&) const;
	virtual size_t getSetSize() const;
  virtual void reset();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
	AFragmentConstant() {}
	AString m_Constant;
};

#endif //INCLUDED__AFragmentConstant_HPP__
