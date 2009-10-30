/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlAtributes_HPP__
#define INCLUDED__AXmlAtributes_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ANameValuePair.hpp"
#include "AString.hpp"

/*!
Attributes are name value pairs separated by space
 e.g.  name1='value 1' name_2="Someone's value" ...
 Default is '...'  unless ' is inside the value, then "..." is used.

Order of insertion is preserved during output
*/
class ABASE_API AXmlAttributes : public ADebugDumpable
{
public:
  typedef LIST_NVPairPtr CONTAINER;

public:
  //! ctor
  AXmlAttributes();

  /*!
  copy ctor
  
  @param that other object
  */
  AXmlAttributes(const AXmlAttributes& that);
  
  /*!
  Of the form "a='something' b=1 c='Hello World'"

  @param strLineToParse attributes line to parse
  */
  AXmlAttributes(const AString& strLineToParse);

  /*!
  dtor
  */
  virtual ~AXmlAttributes();

  /*!
  Copy operator

  @param that other object to copy
  @return reference to this
  */
  AXmlAttributes& operator= (const AXmlAttributes& that);
  
  /*!
  A way to add two AXmlAttribute types together

  @param that other object to append from
  */
  void append(const AXmlAttributes& that);
  
  /*!
  Parse a line of attributes "a='1' b=2 c='foo' ..."
  
  @param strLine to parse
  */
  virtual void parse(const AString& strLine);
  
  /*!
  AXmlEmittable
  
  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  Get attribute value

  @param name of the attribute can be either attrname or @attrname, leading @ will be stripped
  @param target to append value to
  @return true if found
  */
  bool get(const AString& name, AOutputBuffer& target) const;

  /*!
  Check if attribute exists

  @param name of the attribute can be either attrname or @attrname, leading @ will be stripped
  @return true if found
  */
  bool exists(const AString& name) const;
  
  /*!
  Gets value as a boolean or returns defaultValue if not found

  @param name of the attribute
  @param defaultValue to use if not found
  @return true if found
  */
  bool getBool(const AString& name, bool defaultValue = false) const;
  
  /*!
  Insert attribute name/value pair

  @param name of the attribute
  @param value of the attribute
  */
  void insert(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);
  
  /*!
  Remove attribute pair

  @param name of the attribute
  @return true if found
  */
  bool remove(const AString& name);
  
  /*!
  Attribute count

  @return number of attributes
  */
  size_t size() const;

  /*!
  Clear the contents
  */
  virtual void clear();

  /*!
  Get all the name entries

  @param target container set for all the names of the attributes
  @return Number of names added to set
  */
  size_t getNames(SET_AString& target) const;

  /*!
  Read-only access to attribute container

  @return constant reference to the attribute container
  */
  const AXmlAttributes::CONTAINER& getAttributeContainer() const;

  /*!
  ASerializable
  
  @param aFile to write to
  */
	virtual void toAFile(AFile& aFile) const;

  /*!
  ASerializable
  
  @param aFile to read from
  */
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Order is important in some cases for attributes and name can map to more than one value
  AXmlAttributes::CONTAINER m_Pairs;

  //! Find pair
  ANameValuePair *_findPair(const AString& name);
  const ANameValuePair *_findPair(const AString& name) const;
};


#endif
