/*
Written by Alex Chachanashvili

Id: $Id$
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
  /*!
  Ctors and dtor
  */
  AXmlAttributes();
  AXmlAttributes(const AXmlAttributes&);
  AXmlAttributes(const AString& strLineToParse);      //a_Of the form "a='something' b=1 c='Hello World'"
  virtual ~AXmlAttributes();

  /*!
  A way to add two AXmlAttribute types together
  */
  void append(const AXmlAttributes&);
  
  /*!
  Parse a line of attributes "a='1' b=2 c='foo' ..."
  */
  virtual void parse(const AString& strLine);
  
  /*!
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Get attribute value
  */
  bool get(const AString& name, AOutputBuffer&) const;
  
  /*!
  Gets value as a boolean or returns defaultValue if not found
  */
  bool getBool(const AString& name, bool defaultValue = false) const;
  
  /*!
  Insert name/value pair
  */
  void insert(const AString& name, const AString &value = AConstant::ASTRING_EMPTY);
  
  /*!
  Remove pair
  */
  void remove(const AString& name);
  
  /*!
  Size of the pairs
  */
  size_t size() const;

  /*!
  Clear the contents
  */
  virtual void clear();

  /*!
  Get all the name entries

  @param Target container set for all the names of the attributes
  @return Number of names added to set
  */
  size_t getNames(SET_AString&) const;

  /*!
  Read-only access to attribute container
  */
  const AXmlAttributes::CONTAINER& getAttributeContainer() const;

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AXmlAttributes::CONTAINER m_Pairs;      //a_Order is important in some cases for attributes and name can map to more than one value

  //a_Find pair
  ANameValuePair *_findPair(const AString& name);
  const ANameValuePair *_findPair(const AString& name) const;
};


#endif
