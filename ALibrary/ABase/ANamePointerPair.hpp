#ifndef INCLUDED__ANamePointerPair_HPP__
#define INCLUDED__ANamePointerPair_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"

class ABASE_API ANamePointerPair : public ADebugDumpable
{
public:
  //a_Creation of a simple name/value pair
  ANamePointerPair(const AString& strName = AConstant::ASTRING_EMPTY, void *pValue = NULL);
  ANamePointerPair(const ANamePointerPair& nvSource);
  virtual ~ANamePointerPair() {}

  //a_Access methods
  const AString& getName()    const { return mstr__Name; }
  void*          getPointer() const { return mp__Value; }
  
  //a_Comparisson methods
  bool isName      (const AString& str) const;
  bool isNameNoCase(const AString& str) const;

  //a_Methods of change
  void setNamePointer(const AString& strName, void *pValue); //a_Assumes the pairs are valid
  void setPointer(void *pValue);

  //a_Represent as a string of form 'name=0xaddress'
  virtual void emit(AOutputBuffer&) const;

  //a_Validation
  bool isValid() const { return (mstr__Name.getSize() > 0 ? true : false); }

  //a_Clear this object
  void clear();
  
  //a_Comparisson
  int compare(const ANamePointerPair& nvSource) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_The NAME=VALUE data
  AString mstr__Name;
  void   *mp__Value;
};

#endif
