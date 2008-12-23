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

The path parameter is NOT xpath, just a simple version to access the element or attribute

All searching assumes (if this is /root and has child0 and child1):
absolute path must include this element (/root/child1)
relative path starts with contained elements and omits this element (child1)

All content added as pointer is owned and deleted by this element
Items added by reference are either cloned or emitted to data
*/
class ABASE_API AXmlElement : public ADebugDumpable, public AXmlEmittable, public AJsonEmittable
{
public:
  //! The way the data will be encoded
  enum Encoding
  {
    ENC_NONE = 0,     //!< Text as is
    ENC_URL,          //!< Url encoded text
    ENC_CDATASAFE,    //!< <![CDATA[ data ]]> with data made safe for CData
    ENC_CDATADIRECT,  //!< <![CDATA[ data ]]> with data as is
    ENC_XMLSAFE,      //!< Replaces < > &  with &lt; &gt; &amp;
    ENC_BASE64,       //!< Encodes with Base64
    ENC_CDATAHEXDUMP  //!< Debugging format useful for examining binary data
  };

public:
  //! Node container
  typedef std::list<AXmlElement *> CONTAINER;
  //! Constant node container
  typedef std::list<const AXmlElement *> CONST_CONTAINER;

  /*! 
  Default ctor
  @param pParent to attach this element to
  */
  AXmlElement(AXmlElement *pParent = NULL);
  
  /*!
  ctor with name
  
  @param name to assign to this element
  @param pParent to attach this element to
  */
  AXmlElement(const AString& name, AXmlElement *pParent = NULL);
  
  /*! 
  ctor with element name and attributes
  
  @param name to assign to this element
  @param attrs to attaching to this element
  @param pParent to attach this element to
  */
  AXmlElement(const AString& name, const AXmlAttributes& attrs, AXmlElement *pParent = NULL);
  
  /*! 
  Copy ctor will clone content recursively
  
  @param that other element
  @param pParent to attach this element to
  */
  AXmlElement(const AXmlElement& that, AXmlElement *pParent = NULL);
  
  //! dtor
  virtual ~AXmlElement();

  /*!
  Get attributes
  
  @return constant reference to the attributes object
  */
  const AXmlAttributes& getAttributes() const;

  /*!
  Use attributes
  
  @return reference to the attributes object
  */
  AXmlAttributes& useAttributes();

  /*!
  Get name of this element
  
  @return constant reference to the element name
  */
  const AString& getName() const;

  /*!
  Use name of this element
  
  @return reference to the element name
  */
  AString& useName();

  /*!
  Returns true if exists

  @param path to check
  @return true if element at path is found
  */
  bool exists(const AString& path) const;

  /*!
  Removes element and all children

  @param path to lookup
  @return true if found and removed
  */
  bool remove(const AString& path);
  
  /*!
  Checks if this element contains child elements

  @return true if this element contains other elements
  */
  bool hasElements() const;

  /*!
  Checks if an element type
  
  @return true if this is an AXmlElement type
  */
  virtual bool isElement() const;

  /*!
  Checks if data type

  @return true if this is an AXmlData type
  @see AXmlData
  */
  virtual bool isData() const;

  /*!
  Checks if instruction

  @return true if this is an AXmlInstruction type
  @see AXmlInstruction
  */
  virtual bool isInstruction() const;

  /*!
  Emitted data corresponding to the path
  
  @param path to look up
  @param target to append to
  @return true if exists
  */
  bool emitString(const AString& path, AOutputBuffer& target) const;

  /*!
  AString of content at the path
  This is not the most efficient call due to temporary return object, but useful for initializing statics
  
  @param path to look up
  @param strDefault to use if path does not refer to an existing element
  @return emitted data into a string for a path or default
  */
  AString getString(const AString& path, const AString& strDefault) const;

  /*!
  int at from content at the path
  
  @param path to look up
  @param iDefault default value if path does not refer to an existing element
  @return result that is converted to int if does not exist default is returned
  */
  int getInt(const AString& path, int iDefault) const;

  /*!
  size_t from content at the path

  @param path to look up
  @param iDefault default value if path does not refer to an existing element
  @return result that is converted to size_t if does not exist default is returned
  */
  size_t getSize_t(const AString& path, size_t iDefault) const;

  /*!
  u4 from content at the path

  @param path to look up
  @param iDefault default value if path does not refer to an existing element
  @return result that is converted to u4 if does not exist default is returned
  */
  u4 getU4(const AString& path, u4 iDefault) const;

  /*!
  u8 from content at the path

  @param path to look up
  @param iDefault default value if path does not refer to an existing element
  @return result that is converted to u8 if does not exist default is returned
  */
  u8 getU8(const AString& path, u8 iDefault) const;

  /*!
  bool from content at the path

  @param path to look up
  @param boolDefault default value if path does not refer to an existing element
  @return result that is converted to bool if does not exist default is returned
  */
  bool getBool(const AString& path, bool boolDefault) const;

  /*!
  Saves value into DOM at path, overwrites existing

  @param path to look up
  @param value to set
  @param encoding to use
  */
  void setString(const AString& path, const AString& value, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Saves value into DOM at path, overwrites existing

  @param path to look up
  @param value to set
  */
  void setInt(const AString& path, int value);

  /*!
  Saves value into DOM at path, overwrites existing

  @param path to look up
  @param value to set
  */
  void setSize_t(const AString& path, size_t value);

  /*!
  Saves value into DOM at path, overwrites existing
  Converts bool to AString

  @param path to look up
  @param value to set
  */
  void setBool(const AString& path, bool value);

  /*!
  Find path based on this element as root
  
  @param path to look up
  @return reference to the AXmlElement or NULL if not found
  */
  AXmlElement *findElement(const AString& path);

  /*!
  Find path based on this element as root
  
  @param path to look up
  @return constant reference to the AXmlElement or NULL if not found
  */
  const AXmlElement *findElement(const AString& path) const;

  /*!
  Searching for path
  If this element is 'a' and contains "b/bb,c/cc@foo",d/dd
  The "/a/c/cc@foo" will be found.  Current element must be first element of the path if absolute
  If path is relative then c/cc will find 2nd child cc in child c
  Adds const AXmlElement* to the result container, will not clear the result, will append

  @param path to look up
  @param result to append 'const AXmlElement' references to
  @return number of elements found
  */
  size_t find(const AString& path, CONST_CONTAINER& result) const;

  /*!
  Create a new element

  NOTE: the reference returned is to the NEWLY ADDED element and not the current element as with other non-add methods

  @param path to create/add
  @return the newly added element
  */
  AXmlElement& addElement(const AString& path);

  /*!
  Overwrite an element or create new one if it does not exist

  NOTE: If path is found the reference returned is to the overwritten element and not the current element as with other non-add methods
  NOTE: If path not found the reference returned is to the NEWLY ADDED element and not the current element as with other non-add methods

  @param path to create/overwrite
  @return the overwritten/added element
  */
  AXmlElement& overwriteElement(const AString& path);

  /*!
  Adds a new element
  
  NOTE: If path is found the reference returned is to the overwritten element and not the current element as with other non-add methods
  NOTE: If path not found the reference returned is to the NEWLY ADDED element and not the current element as with other non-add methods

  @param path to look up
  @param object to add as a string (emit will be called on it)
  @param encoding to use on added string
  @param overwrite - will try to find existing structure to replace, otherwise will create new path
  @return the newly added element
  */
  AXmlElement& addElement(const AString& path, const AEmittable& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, bool overwrite = false);

  /*!
  Add AEmittable type (encoded as needed)

  @param object to emit to content data of this element
  @param encoding to use
  @return this object
  */
  AXmlElement& addData(const AEmittable& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Add const char * value to text/data of element

  @param value string to add to content data of this element
  @param length of the value to add
  @param encoding to use
  @return this object
  */
  AXmlElement& addData(const char *value, size_t length = AConstant::npos, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Add signed int value to text/data of element (no encoding)

  @param value (int as string) to add to content data of this element
  @return this object
  */
  AXmlElement& addData(const int value);
  
  /*!
  Add char value entered as is (no encoding)

  @param value char to add to content data of this element
  @return this object
  */
  AXmlElement& addData(const char value);

  /*!
  Add double value to text/data of element (no encoding)

  @param value (double as string) to add to content data of this element
  @return this object
  */
  AXmlElement& addData(const double value);

  /*!
  Add std::size_t value to text/data of element (no encoding)

  @param value (size_t as string) to add to content data of this element
  @return this object
  */
  AXmlElement& addData(const size_t value);
    
  /*!
  Set AEmittable type (encoded as needed)

  @param object to emit to set as content data of this element
  @param encoding to use
  @return this object
  */
  AXmlElement& setData(const AEmittable& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Set const char * value to text/data of element

  @param value string to set as content data of this element
  @param length of the value to set
  @param encoding to use
  @return this object
  */
  AXmlElement& setData(const char *value, size_t length = AConstant::npos, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Set signed int value to text/data of element (no encoding)

  @param value (int as string) to add to content data of this element
  @return this object
  */
  AXmlElement& setData(const int value);
  
  /*!
  Set char value entered as is (no encoding)

  @param value char to set to content data of this element
  @return this object
  */
  AXmlElement& setData(const char value);

  /*!
  Set double value to text/data of element (no encoding)

  @param value (double as string) to set to content data of this element
  @return this object
  */
  AXmlElement& setData(const double value);

  /*!
  Set std::size_t value to text/data of element (no encoding)

  @param value (size_t as string) to add to content data of this element
  @return this object
  */
  AXmlElement& setData(const size_t value);

  /*!
  Adds an XML/HTML comment

  @param comment to add
  @return this object
  */
  AXmlElement& addComment(const AString& comment);

  /*!
  Adds content element pointer
    this object will OWN the passed content pointer and release it when done

  If path is empty or "/", current element is used as base
  For valid path, elements will be added to this one to extend the path

  @param pContent to add to this element (will be OWNED by this element and deleted when done)
  @param path to add to
  @return this object
  */
  AXmlElement& addContent(AXmlElement *pContent, const AString& path = AConstant::ASTRING_EMPTY);
  
  /*!
  AXmlEmittable added as content to the element
  that.emitXml(*this) is called
  
  @param that other object to XML emit into current element
  @return this object
  */
  AXmlElement& addContent(const AXmlEmittable& that);

  /*!
  Add attribute (replaces any existing)
  
  @param attrs to add
  @return this object
  */
  AXmlElement& addAttributes(const AXmlAttributes& attrs);

  /*!
  Add attribute (replaces any existing)
  
  @param name of the attribute
  @param value of the attribute
  @return this object
  */
  AXmlElement& addAttribute(const AString& name, const AString& value = AConstant::ASTRING_EMPTY);

  /*!
  Add attribute (replaces any existing)
  
  @param name of the attribute
  @param value (converted to string) of the attribute
  @return this object
  */
  AXmlElement& addAttribute(const AString& name, const u4 value);

  /*!
  Add attribute (replaces any existing)
  
  @param name of the attribute
  @param value (converted to string) of the attribute
  @return this object
  */
  AXmlElement& addAttribute(const AString& name, const u8 value);

  /*!
  Add attribute (replaces any existing)
  
  @param name of the attribute
  @param value (converted to string) of the attribute
  @return this object
  */
  AXmlElement& addAttribute(const AString& name, const double value);

  /*!
  AEmittable

  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AEmittable and other output methods

  @param target to append to
  @param indent >= 0 will make it human-readable by adding indent and CRLF
  */
  virtual void emit(AOutputBuffer& target, int indent) const;

  /*!
  AJsonEmittable

  @param target to emit to
  @param indent -1=none, >=0 for indent at each level, each recursive call increases indent internally
  */
  virtual void emitJson(AOutputBuffer& target, int indent = -1) const;
  
  /*!
  Emit content only, unecoded raw data

  @param target to emit to
  @return true if there was some content, false if no content or singular element
  */
  virtual bool emitContent(AOutputBuffer& target) const;

  /*!
  Emit Xml from path
  
  @param path to start emit at
  @param target to emit to
  @param indent -1=none, >=0 for indent at each level, each recursive call increases indent internally
  @return false if path not found
  */
  bool emitXmlFromPath(const AString& path, AOutputBuffer& target, int indent = -1) const;

  /*!
  Emit content from path

  @param path to start emit at
  @param target to emit to
  @return false if path not found
  */
  bool emitContentFromPath(const AString& path, AOutputBuffer& target) const;

  /*!
  AXmlEmittable
  Adds this element with all its content to thisRoot

  @param thisRoot to emit XML into
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Emit XML content of this element into the given element (does not include the current element)

  @param rootThis to emit XML content into
  @return rootThis for convenience
  */
  virtual void emitXmlContent(AXmlElement& rootThis) const;

  /*!
  Emit the path of the current element based on absolute root
  Will traverse up parents until NULL parent is reached

  @param target to emit to
  */
  virtual void emitPath(AOutputBuffer& target) const;

  /*!
  Clear the content and attributes
  Detaches from parent
  NOTE: Does NOT clear the name of the object
  */
  virtual void clear();

  /*!
  Clears the content only
  */
  void clearContent();

  /*!
  Content
  List of AXmlElement objects contained in this one

  @return container of the content elements
  */
  const AXmlElement::CONTAINER& getContentContainer() const;

  /*!
  Get parent element
  
  @return parent element or NULL if none
  */
  AXmlElement *getParent() const { return mp_Parent; }

  /*!
  Set parent element
  
  @param pParent new parent element or NULL if none
  @return current parent element
  */
  AXmlElement *setParent(AXmlElement *pParent);

  /*!
  Clone of self used in deep copy

  @return new cloned object and copies of all the content/elements
  */
  virtual AXmlElement* clone() const;

  /*!
  Parsing XML from file
  NOTE: assumes actual XML element, not data or instruction
  Use AXmlDocument to parse entire XML documents
  
  @param aFile to read from
  */
  virtual void fromAFile(AFile& aFile);
  
  /*!
  Parsing XML to file
  NOTE: assumes actual XML element, not data or instruction
  Use AXmlDocument to parse entire XML documents
  
  @param aFile to read from
  */
  virtual void toAFile(AFile& aFile) const;

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
