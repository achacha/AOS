#ifndef INCLUDED__AXmlElement_HPP__
#define INCLUDED__AXmlElement_HPP__

#include "apiABase.hpp"
#include "AXmlNode.hpp"
#include "AXmlData.hpp"

class AOutputBuffer;
class AEmittable;
class AObjectBase;

class ABASE_API AXmlElement : public AXmlNode
{
public:
  /*!
  If element has no name it will only emit its children (which may not be a valid Xml doc by itself, well formed Xml needs to have one root element)
  AAttributes can be used to specify attributes with this element (assuming it has a name, if it doesn't they will be ignored during output)
  */
  AXmlElement(AXmlNode *pParent = NULL);
  AXmlElement(const AString& name, AXmlNode *pParent = NULL);
  AXmlElement(const AString& name, const AAttributes& attrs, AXmlNode *pParent = NULL);
  AXmlElement(const AXmlElement&, AXmlNode *pParent = NULL);
  virtual ~AXmlElement();

  /*!
  Adds a new element
  
  insert - will try to find existing structure, otherwise will create new xpath

  **** NOTE: Add returns NEWLY added element (NOT 'this' as with the other methods) ****
  This is done to allow you to add an element and get a reference to the newly added element
  */
  AXmlElement& addElement(const AString& xpath, const AString& value = AString::sstr_Empty, AXmlData::Encoding encoding = AXmlData::None, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const ARope& value, AXmlData::Encoding encoding = AXmlData::None, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const AEmittable& object, AXmlData::Encoding encoding, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const AXmlEmittable& object, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const char value, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const u4 value, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const u8 value, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const double value, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const size_t value, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const bool value, bool insert = true);
  AXmlElement& addElement(const AString& xpath, const char * value, u4 len = AConstant::npos, AXmlData::Encoding encoding = AXmlData::None, bool insert = true);
  
  /*!
  Adds a comment
  Return this element
  */
  AXmlElement& addComment(const AString&);

  /*!
  Adds content
  Return this element
  */
  AXmlElement& addContent(AXmlNode *);
  AXmlElement& addContent(const AXmlEmittable&);
  AXmlElement& addData(const AEmittable&, AXmlData::Encoding encoding = AXmlData::None);

  /*!
  Add attribute
  Returns this object
  */
  AXmlElement& addAttributes(const AAttributes& attrs);
  AXmlElement& addAttribute(const AString& name, const AString& value = AString::sstr_Empty);
  AXmlElement& addAttribute(const AString& name, const u4 value);
  AXmlElement& addAttribute(const AString& name, const u8 value);
  AXmlElement& addAttribute(const AString& name, const double value);

  /*!
  Checks if this element contains other elements
  */
  bool hasElements() const;

  /*!
  Emit XML unformatted
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Use -1 for all cases processing cases to reduce spaces/CRLF, XSLT/etc parsers don't care about formatting
  indent >=0 will add 2 spaces per indent and make it human-readable
  */
  virtual void emit(AOutputBuffer&, int indent) const;

  /*!
  Clear the contents and release sub-elements
  NOTE: Does NOT clear the name of this object
  */
  virtual void clear();

  /*!
  Clone of self used in deep copy
  */
  virtual AXmlNode* clone() const { return new AXmlElement(*this); }

  /*!
  Parsing XML to and from file (assumes a perfect XML node without instructions, AXmlDocument accounts for instructions)
  */
  virtual void fromAFile(AFile&);
  virtual void toAFile(AFile&) const;

protected:
  /*! Creates a new element */
  AXmlElement *_createAndAppend(LIST_AString& xparts, AXmlNode* pParent);
  
  /*! Attempts to insert */
  AXmlElement *_getAndInsert(LIST_AString& xparts, AXmlNode* pParent);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif
