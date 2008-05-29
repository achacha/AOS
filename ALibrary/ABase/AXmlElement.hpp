/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlElement_HPP__
#define INCLUDED__AXmlElement_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlAttributes.hpp"
#include "AXmlEmittable.hpp"
#include "AJsonEmittable.hpp"

/*!
Base class that represents an XML element node 

All searching assumes (if this is /root and has child0 and child1):
absolute path must include this element (/root/child1)
relative path starts with contained elements and omits this element (child1)

All content added as pointer is owned and deleted by this element
Items added by reference are either cloned or emitted to data
*/
class ABASE_API AXmlElement : public ADebugDumpable, public AXmlEmittable, public AJsonEmittable
{
public:
  //a_The way the data will be encoded
  enum Encoding
  {
    ENC_NONE = 0,     //a_Text as is
    ENC_URL,          //a_Url encoded text
    ENC_CDATASAFE,    //a_<![CDATA[ data ]]> with data made safe for CData
    ENC_CDATADIRECT,  //a_<![CDATA[ data ]]> with data as is
    ENC_XMLSAFE,      //a_Replaces < > &  with &lt; &gt; &amp;
    ENC_BASE64,       //a_Encodes with Base64
    ENC_CDATAHEXDUMP  //a_Debugging format useful for examining binary data
  };

public:
  typedef std::list<AXmlElement *> CONTAINER;
  typedef std::list<const AXmlElement *> CONST_CONTAINER;

  //! default ctor
  AXmlElement(AXmlElement *pParent = NULL);
  
  //! ctor with name
  AXmlElement(const AString& name, AXmlElement *pParent = NULL);
  
  //! ctor with name and attributes
  AXmlElement(const AString& name, const AXmlAttributes& attrs, AXmlElement *pParent = NULL);
  
  //! Copy ctor will clone content recursively
  AXmlElement(const AXmlElement& that, AXmlElement *pParent = NULL);
  
  //! dtor
  virtual ~AXmlElement();

  /*!
  Get attributes
  */
  const AXmlAttributes& getAttributes() const;
  AXmlAttributes& useAttributes();

  /*!
  Name of this element
  */
  const AString& getName() const;
  AString& useName();

  /*!
  Returns true if exists
  */
  bool exists(const AString& path) const;

  /*!
  Removes element and all children
  */
  bool remove(const AString& path);

  /*!
  Checks if this element contains child elements
  */
  bool hasElements() const;

  /*!
  Checks if an element type
  */
  virtual bool isElement() const;

  /*!
  Checks if data type
  */
  virtual bool isData() const;

  /*!
  Checks if instruction
  */
  virtual bool isInstruction() const;

  /*!
  Emitted data corresponding to the path
  Result is appended
  Returns true if exists
  */
  bool emitString(const AString& path, AOutputBuffer& target) const;

  /*!
  String corresponding to the path
  This is not the most efficient call due to temporary return object, but useful for initializing statics
  Returns emitted data into a string for a path or default
  */
  AString getString(const AString& path, const AString& strDefault) const;

  /*!
  String corresponding to the path
  Result is converted to int if does not exist default is returned
  */
  int getInt(const AString& path, int iDefault) const;

  /*!
  String corresponding to the path
  Result is converted to size_t if does not exist default is returned
  */
  size_t getSize_t(const AString& path, size_t iDefault) const;

  /*!
  String corresponding to the path
  Result is converted to u4 if does not exist default is returned
  */
  u4 getU4(const AString& path, u4 iDefault) const;

  /*!
  String corresponding to the path
  Result is converted to u8 if does not exist default is returned
  */
  u8 getU8(const AString& path, u8 iDefault) const;

  /*!
  String corresponding to the path
  Case insensitive "true" or "1" are true
  Result is converted to bool if does not exist default is returned
  */
  bool getBool(const AString& path, bool boolDefault) const;

  /*!
  Saves value into DOM at path, overwrites existing
  */
  void setString(const AString& path, const AString& value, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Saves value into DOM at path, overwrites existing
  */
  void setInt(const AString& path, int value);

  /*!
  Saves value into DOM at path, overwrites existing
  */
  void setSize_t(const AString& path, size_t value);

  /*!
  Saves value into DOM at path, overwrites existing
  Converts bool to AString
  */
  void setBool(const AString& path, bool value);

  /*!
  Find path based on this element as root
  Returns NULL if not found
  */
  AXmlElement *findElement(const AString& xpath);
  const AXmlElement *findElement(const AString& xpath) const;

  /*!
  Searching for path
  If this element is 'a' and contains "b/bb,c/cc@foo",d/dd
  The "/a/c/cc@foo" will be found.  Current element must be first element of the path if absolute
  If path is relative then c/cc will find 2nd child cc in child c
  Adds const AXmlElement* to the result container, will not clear the result, will append

  Returns number of elements found
  */
  size_t find(const AString& path, CONST_CONTAINER& result) const;

  /*!
  Create a new element

  Returns the NEW element
  */
  AXmlElement& addElement(const AString& path);

  /*!
  Overwrite an element or create new one if it does not exist

  Returns the overwritten element
  */
  AXmlElement& overwriteElement(const AString& path);

  /*!
  Adds a new element
  
  overwrite - will try to find existing structure to replace, otherwise will create new path

  returns: NEW element

  **** NOTE: Add returns NEWLY added element (NOT 'this' as with the other methods) ****
  This is done to allow you to add an element and get a reference to the newly added element
  */
  AXmlElement& addElement(const AString& path, const AEmittable& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, bool overwrite = false);

  /*!
  Add AEmittable type (encoded as needed)
  */
  AXmlElement& addData(const AEmittable& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Add const char * value to text/data of element
  */
  AXmlElement& addData(const char *value, size_t length = AConstant::npos, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Add signed int value to text/data of element (no encoding)
  */
  AXmlElement& addData(const int value);
  
  /*!
  Add char value entered as is (no encoding)
  */
  AXmlElement& addData(const char value);

  /*!
  Add double value to text/data of element (no encoding)
  */
  AXmlElement& addData(const double value);

  /*!
  Add std::size_t value to text/data of element (no encoding)
  */
  AXmlElement& addData(const size_t value);
    
  /*!
  Add AEmittable type (encoded as needed)
  */
  AXmlElement& setData(const AEmittable& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Set const char * value to text/data of element
  */
  AXmlElement& setData(const char *value, size_t length = AConstant::npos, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Set signed int value to text/data of element (no encoding)
  */
  AXmlElement& setData(const int value);
  
  /*!
  Set char value entered as is (no encoding)
  */
  AXmlElement& setData(const char value);

  /*!
  Set double value to text/data of element (no encoding)
  */
  AXmlElement& setData(const double value);

  /*!
  Set std::size_t value to text/data of element (no encoding)
  */
  AXmlElement& setData(const size_t value);

  /*!
  Adds a comment
  Return this element
  */
  AXmlElement& addComment(const AString&);

  /*!
  Adds content element pointer
    this object will OWN the passed content pointer and release it when done

  If path is empty or "/", current element is used as base
  For valid path, elements will be added to this one to extend the path

  Returns this element
  */
  AXmlElement& addContent(AXmlElement *pContent, const AString& path = AConstant::ASTRING_EMPTY);
  
  /*!
  Adds XML emittable as content to the element
  Returns this element
  */
  AXmlElement& addContent(const AXmlEmittable&);

  /*!
  Add attribute (replaces any existing attribute names must be unique)
  Returns this element
  */
  AXmlElement& addAttributes(const AXmlAttributes& attrs);
  AXmlElement& addAttribute(const AString& name, const AString& value = AConstant::ASTRING_EMPTY);
  AXmlElement& addAttribute(const AString& name, const u4 value);
  AXmlElement& addAttribute(const AString& name, const u8 value);
  AXmlElement& addAttribute(const AString& name, const double value);

  /*!
  AEmittable and other output methods
   indent >= 0 will make it human-readable by adding indent and CRLF
   emit will include current element, emitContent will not
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer&, int indent) const;

  /*!
  AJsonEmittable
  */
  virtual void emitJson(AOutputBuffer&, int indent = -1) const;
  
  /*!
  Emit content only, unecoded raw data
  */
  virtual void emitContent(AOutputBuffer&) const;

  /*!
  Emit Xml from path
  returns false if path not found
  */
  bool emitXmlFromPath(const AString& path, AOutputBuffer&, int indent = -1) const;

  /*!
  Emit content from path
  returns false if path not found
  */
  bool emitContentFromPath(const AString& path, AOutputBuffer&) const;

  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Emit XML content into the given element (does not include current element unline emitXml())
  */
  virtual void emitXmlContent(AXmlElement&) const;

  /*!
  Clear the object and release sub-elements
  NOTE: Does NOT clear the name of the object
  */
  virtual void clear();

  /*!
  Content
  List of AXmlElement objects contained in this one
  */
  const AXmlElement::CONTAINER& getContentContainer() const;

  /*!
  Set/Get parent element, NULL if none
  */
  AXmlElement *getParent() const { return mp_Parent; }
  void setParent(AXmlElement *pParent) { mp_Parent = pParent; }

  //a_Clone of self used in deep copy
  virtual AXmlElement* clone() const;

  /*!
  Parsing XML to and from file
  NOTE: assumes a actual XML element, not data or instruction
  Use AXmlDocument to parse entire XML documents
  */
  virtual void fromAFile(AFile&);
  virtual void toAFile(AFile&) const;

protected:
  //! Element name
  AString m_Name;

  //! Content container
  CONTAINER m_Content;

  //! XML attributes
  AXmlAttributes m_Attributes;

  //! Parent element (NULL if none)
  AXmlElement *mp_Parent;

  //! Recursive search function
  AXmlElement *_get(LIST_AString& xparts) const;

  //! Indents with 2 spaces per indent
  inline void _indent(AOutputBuffer&, int) const;

  //! Find method for non const elements
  size_t _find(const AString& path, CONTAINER& result);

  //! Internal find
  size_t _const_find(LIST_AString listPath, AXmlElement::CONST_CONTAINER& result) const;

  //! Internal find
  size_t _nonconst_find(LIST_AString listPath, AXmlElement::CONTAINER& result);

  //! Add element
  AXmlElement *_addElement(const AString& path, bool overwrite);

  //! Remove child element
  void _removeChildElement(AXmlElement *);

  //! Add data to this element
  //! overwrite of true will clar existing content and replace it with value
  void _addData(const AEmittable& value, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, bool overwrite = false);

  //! Creates a new element
  AXmlElement *_createAndAppend(LIST_AString& xparts, AXmlElement* pParent);
  
  //! Attempts to overwrite if not found then creates
  AXmlElement *_getOrCreate(LIST_AString& xparts, AXmlElement* pParent);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif

public:
  //a_For convenience and efficiency
  static const AString sstr_Start;                         //a_ "<"
  static const AString sstr_StartComment;                  //a_ "<!--"
  static const AString sstr_StartInstruction;              //a_ "<?"
  static const AString sstr_StartEnd;                      //a_ "</"
  static const AString sstr_EndSingular;                   //a_ "/>"
  static const AString sstr_End;                           //a_ ">"
  static const AString sstr_EndOrWhitespace;               //a_ "/> \t\n\r"
  static const AString sstr_EndInstruction;                //a_ "?>"
  static const AString sstr_EndComment;                    //a_ "-->"
  static const AString sstr_EndInstructionOrWhitespace;    //a_ "?> \t\n\r"
};

#endif // INCLUDED__AXmlElement_HPP__
