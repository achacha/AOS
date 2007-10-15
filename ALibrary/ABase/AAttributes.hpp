#ifndef INCLUDED__AAtributes_HPP__
#define INCLUDED__AAtributes_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ANameValuePair.hpp"
#include "AString.hpp"

class AAttributes;

typedef std::map<AString, AAttributes> MAP_AString_AAttributes;

/*!
Attributes are name value pairs separated by space
 e.g.  name1='value 1' name_2="Someone's value" ...
 Default is '...'  unless ' is inside the value, then "..." is used.
*/
class ABASE_API AAttributes : public ADebugDumpable
{
public:
  //a_Ctors and dtor
  AAttributes();
  AAttributes(const AAttributes&);
  AAttributes(const AString& strLineToParse);      //a_Of the form "a='something' b=1 b=3 b=6 c='Hello World'"
  virtual ~AAttributes();

  //a_A way to add two AAttribute types together
  //a_Duplicate names generated when values are different (i.e. somename -> { value1, value2, value3, ... })
  void operator |=(const AAttributes& that) { append(that); }
  void append(const AAttributes&);

  //a_Clear the contents
  virtual void clear();
  
  //a_Parse a line of attributes "a='1' b=2 c='foo' ..."
  virtual void parse(const AString& strLine, ANameValuePair::NameValueType eType = ANameValuePair::XML);
  
  //a_Output
  virtual void emit(AOutputBuffer&) const;

  //a_Access based on name
  bool   get(const AString& strName, AOutputBuffer&, bool boolNoCase = true) const;     //a_NOTE: gets the first one it finds
  size_t get(const AString& strName, LIST_AString& result, bool boolNoCase = true) const;
  size_t count(const AString& strName, bool boolNoCase = true) const;
  void   insert(const AString& strName, const AString &strValue = AConstant::ASTRING_EMPTY, ANameValuePair::NameValueType eType = ANameValuePair::XML);
  size_t getDelimited(const AString& strName, AString& strValues, const AString& delimeter = AConstant::ASTRING_COMMA, bool boolNoCase = true) const;     //a_Will generate a string "value1,value2,value3..."
  void   remove(const AString& strName, const AString &strValue = AConstant::ASTRING_EMPTY, bool boolNoCase = true);  //no value specified means remove the key and all values
  size_t size() const;

  /**
   * Get all the name entries in the vector (no duplicates checked)
  **/
  size_t getNames(SET_AString&, bool boolNoCase = true) const;

  /*!
  Read-only access to attribute container
  */
  const LIST_NVPair& getAttributeContainer() const;

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

private:
  LIST_NVPair m_Pairs;      //a_Order is important in some cases for attributes and name can map to more than one value

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};


#endif
