#ifndef INCLUDED__AQueryString_HPP__
#define INCLUDED__AQueryString_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "ANameValuePair.hpp"

/*!
AQueryString are name value pairs separated by &
  e.g.  name1=value%201&name_2=Some%20other%20value ...etc...

  name is always case insensitive and forced to lower case
*/
class ABASE_API AQueryString : public ADebugDumpable, public AXmlEmittable
{
public:
  //a_Ctors and dtor
  AQueryString();
  AQueryString(const AString& strLine);
  AQueryString(const AQueryString& AFormSource);
  virtual ~AQueryString();

  //a_Assignment operator (overwrite assumed)
  AQueryString &operator =(const AQueryString& that) { copy(that); return *this; }
  void copy(const AQueryString&);

  //a_A way to add two query strings together
  //a_Duplicate names generated when values are different
  void operator |=(const AQueryString& that) { append(that); }
  void append(const AQueryString&);

  void clear();
  
  /*!
  Parse string, can specify an alternate pairs types supported by ANameValuePair
  */
  void parse(const AString& strLine, ANameValuePair::NameValueType type = ANameValuePair::CGI);

  /*!
  AEmittable and AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AXmlElement&) const;

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
  Insert name/value pair
  */
  virtual void insert(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);
  
  /*!
  Remove name and all of its values
  If value specified only that value is removed
  */
  virtual void remove(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);  //no value specified means remove the key and all values
  
  /*!
  Count how many of a given name exist
  */
  virtual size_t count(const AString& name) const;

  /*!
  Check if name exists
  */
  virtual bool exists(const AString& name) const;

  /*!
  Gets the first one it finds, but there may be more
  Returns true if found, else false
  Call to count can be used if not sure how many values exist
  */
  virtual bool get(const AString& name, AString&) const;

  /*!
  Get a list of all values associated with a given name
  Returns # found (same as count)
  */
  virtual size_t get(const AString& name, LIST_NVPair& result) const;

  /*!
  Get values delimeted
  */
  virtual size_t getDelimited(const AString& name, AString& values, const AString& delimeter = AConstant::ASTRING_COMMA) const;     //a_Will generate a string "value1,value2,value3..."

  /*!
  Number of values
  */
  size_t size() const;

  /*!
  All unique names
  */
  size_t getNames(SET_AString&) const;

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

protected:
  MMAP_AString_NVPair m_Pairs;      //a_Storage of name value pairs  name -> ANameValuePair, ANameValuePair ... 

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};


#endif // INCLLUDED__AQueryString_HPP__
