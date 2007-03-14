#ifndef INCLUDED__ALogicStatement_HPP__
#define INCLUDED__ALogicStatement_HPP__

#include "apiABase.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"

class AFile;

/*!
This is meant to be used to get a rough average
 when the number of samples or value of the sum for the average
 may overflow the machine based numbers
 Depending on data fluctuation and weight parameters
 this class can track result fairly accurately
 It should not be used to determine exact results
 Results are repeatable and reproduceable with same data
*/
class ABASE_API ALogicStatement : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  ctor parse
  */
  ALogicStatement(const AString&);
  virtual ~ALogicStatement();

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AXmlElement&) const;

private:
  ALogicStatement() {}

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ALogicStatement_HPP__
