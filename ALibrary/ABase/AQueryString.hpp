/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AQueryString_HPP__
#define INCLUDED__AQueryString_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "ANameValuePair.hpp"

/*!
AQueryString are name value pairs separated by &
  e.g.  name1=value%201&name_2=Some%20other%20value ...etc...

  name is case sensitive (since some servers are case sensitive when parsing query string)
*/
class ABASE_API AQueryString : public ADebugDumpable, public AXmlEmittable
{
public:
  //a_Ctors and dtor
  AQueryString();
  AQueryString(const AString& strLine);
  AQueryString(const AQueryString&);
  virtual ~AQueryString();

  /*!
  Assignment operator (overwrite assumed)

  @param that other AQueryString
  */
  AQueryString &operator =(const AQueryString& that);

  /*!
  Assignment operator (overwrite assumed)

  @param that other AQueryString
  */
  void copy(const AQueryString& that);

  /*!
  A way to add two query strings together
  Duplicate names generated when values are different
  
  @param that other AQueryString
  */
  void operator |=(const AQueryString& that) { append(that); }

  /*!
  A way to add two query strings together
  Duplicate names generated when values are different
  
  @param that other AQueryString
  */
  void append(const AQueryString& that);

  /*!
  Clear the query string
  */
  void clear();
  
  /*!
  Parse string, can specify an alternate pairs types supported by ANameValuePair
  */
  void parse(const AString& strLine, ANameValuePair::NameValueType type = ANameValuePair::CGI_CASE);

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer& target) const;
  
  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Emit delimeted
  
  Given: a=0&b=1&c=2 query string
  
  If valueSeparator='-', pairSeparator=':' and appendSeparatorAtEnd = false
  Then: a-1:b-2:c-3
  
  If valueSeparator='.', pairSeparator="-=-" and appendSeparatorAtEnd = true
  Then: a.1-=-b.2-=-c.3-=-
  */
  void emitDelimeted(
    AOutputBuffer&, 
    const AString& valueSeparator, 
    const AString& pairSeparator, 
    bool appendSeparatorAtEnd = true
  ) const;

  /*!
  Set name/value pair, removing any instance of name existing in the query string

  @param name of pair
  @param value of pair
  */
  void set(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);

  /*!
  Add name/value pair

  @param name of pair
  @param value of pair
  */
  void add(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);
  
  /*!
  Remove name and all of its values
  If value specified only that value is removed

  @param name of pair
  @param value of pair
  */
  void remove(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);  //no value specified means remove the key and all values
  
  /*!
  Count how many of a given name exist

  @param name of pair
  @return number of values found for the given name
  */
  size_t count(const AString& name) const;

  /*!
  Check if name exists

  @param name of pair
  @return true if found
  */
  bool exists(const AString& name) const;

  /*!
  Gets the first one it finds, but there may be more
  Returns true if found, else false
  Call to count can be used if not sure how many values exist

  @param name of the first found pair
  @param target to append to
  @return true if found
  */
  bool get(const AString& name, AString& target) const;

  /*!
  Get a list of all values associated with a given name
  Returns # found (same as count)

  @param name of pair(s)
  @param target to append to
  @return number of values found
  */
  size_t get(const AString& name, LIST_NVPair& target) const;

  /*!
  Get values delimeted

  @param name of pair
  @param value of pair
  @param delimeter to use
  */
  size_t getDelimited(const AString& name, AString& values, const AString& delimeter = AConstant::ASTRING_COMMA) const;     //a_Will generate a string "value1,value2,value3..."

  /*!
  Number of values

  @return number of pairs
  */
  size_t size() const;

  /*!
  All unique names

  @param target to add names to
  @return number of pairs added to set
  */
  size_t getNames(SET_AString& target) const;

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  MMAP_AString_NVPair m_Pairs;      //a_Storage of name value pairs  name -> ANameValuePair, ANameValuePair ... 
};


#endif // INCLLUDED__AQueryString_HPP__
