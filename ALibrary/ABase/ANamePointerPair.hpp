/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ANamePointerPair_HPP__
#define INCLUDED__ANamePointerPair_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"

class ABASE_API ANamePointerPair : public ADebugDumpable
{
public:
  /*!
  ctor

  @param name of the pair
  @param pValue pointer
  */
  ANamePointerPair(const AString& name, void *pValue = NULL);
  
  //! copy ctor
  ANamePointerPair(const ANamePointerPair&);
  
  //! dtor
  virtual ~ANamePointerPair();

  /*!
  Pair name
  
  @return constant refence to AString
  */
  const AString& getName() const;

  /*!
  Pair name
  
  @return refence to AString
  */
  AString& useName();

  /*!
  Get value
  
  @return pointer
  */
  void *getPointer() const;
  
  /*!
  Set value
  
  @param p new pointer value
  */
  void setPointer(void *p);

  //a_Represent as a string of form 'name=0xaddress'
  virtual void emit(AOutputBuffer& target) const;

  /*!
  Checks if valid
  
  @return true if name is not empty
  */
  bool isValid() const;

  //! Clear this object
  void clear();
  
  /*!
  Comparisson

  @return 0 if equal, <0 if less than, >0 if greater than
  */
  int compare(const ANamePointerPair& nvSource) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  // The NAME=VALUE data
  AString m_name;
  void   *mp_value;
};

#endif
