#ifndef INCLUDED__AXmlElement_HPP__
#define INCLUDED__AXmlElement_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AAttributes.hpp"
#include "AXmlEmittable.hpp"
#include "AJsonEmittable.hpp"

/*!
Base class that represents an XML node from which AElement, AData and such are derived
Pure virtual class not meant to be instanciated
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
  typedef std::list<AXmlElement *> NodeContainer;
  typedef std::list<const AXmlElement *> ConstNodeContainer;

  //! default ctor
  AXmlElement(AXmlElement *pParent = NULL);
  
  //! ctor with name
  AXmlElement(const AString& name, AXmlElement *pParent = NULL);
  
  //! ctor with name and attributes
  AXmlElement(const AString& name, const AAttributes& attrs, AXmlElement *pParent = NULL);
  
  //! Copy ctor will clone content recursively
  AXmlElement(const AXmlElement& that, AXmlElement *pParent = NULL);
  
  //! dtor
  virtual ~AXmlElement();

  /*!
  Get attributes
  */
  const AAttributes& getAttributes() const;
  AAttributes& useAttributes();

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
  Case insensitive "true" or "1" are true
  Result is converted to bool if does not exist default is returned
  */
  bool getBool(const AString& path, bool boolDefault) const;

  /*!
  Find path based on this node as root
  Returns NULL if not found
  */
  AXmlElement *findNode(const AString& xpath);
  const AXmlElement *findNode(const AString& xpath) const;

  /*!
  Searching for path
  If this element is 'a' and contains "b/bb,c/cc@foo",d/dd
  The "/a/c/cc@foo" will be found.  Current element must be first element of the path if absolute
  If path is relative then c/cc will find 2nd child cc in child c
  Adds const AXmlElement* to the result container, will not clear the result, will append

  Returns elements found
  */
  size_t find(const AString& path, ConstNodeContainer& result) const;

  /*!
  Adds a new element
  
  insert - will try to find existing structure, otherwise will create new xpath

  **** NOTE: Add returns NEWLY added element (NOT 'this' as with the other methods) ****
  This is done to allow you to add an element and get a reference to the newly added element
  */
  AXmlElement& addElement(const AString& path, const AString& value = AConstant::ASTRING_EMPTY, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, bool insert = false);
  AXmlElement& addElement(const AString& path, const AEmittable& object, AXmlElement::Encoding encoding, bool insert = false);
  AXmlElement& addElement(const AString& path, const char * value, u4 len = AConstant::npos, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, bool insert = false);

//TODO: make this go into addText
  AXmlElement& addElement(const AString& path, const char value, bool insert = false);
  AXmlElement& addElement(const AString& path, const u4 value, bool insert = false);
  AXmlElement& addElement(const AString& path, const u8 value, bool insert = false);
  AXmlElement& addElement(const AString& path, const double value, bool insert = false);
  AXmlElement& addElement(const AString& path, const size_t value, bool insert = false);
  
  /*!
  Adds a comment
  Return this element
  */
  AXmlElement& addComment(const AString&);

  /*!
  Adds content node pointer
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
  Adds text to element (no encoding, for encoding use addData)
  Returns this element
  */
  AXmlElement& addText(const AEmittable&);

  /*!
  Adds data (depending on encoding)
  Returns this element
  */
  AXmlElement& addData(const AEmittable&, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Add attribute
  Returns this object
  */
  AXmlElement& addAttributes(const AAttributes& attrs);
  AXmlElement& addAttribute(const AString& name, const AString& value = AConstant::ASTRING_EMPTY);
  AXmlElement& addAttribute(const AString& name, const u4 value);
  AXmlElement& addAttribute(const AString& name, const u8 value);
  AXmlElement& addAttribute(const AString& name, const double value);

  /*!
  Saves value into DOM at path
  */
  void setString(const AString& path, const AString& value, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);

  /*!
  Saves value into DOM at path
  */
  void setInt(const AString& path, int value);

  /*!
  Saves value into DOM at path
  */
  void setSize_t(const AString& path, size_t value);

  /*!
  Saves value into DOM at path
  Converts bool to "true" or "false"
  */
  void setBool(const AString& path, bool value);

  /*!
  AEmittable and other output methods
   indent >= 0 will make it human-readable by adding indent and CRLF
   emit will include current node, emitContent will not
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
  bool emitFromPath(const AString& path, AOutputBuffer&, int indent = -1) const;

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

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
  const AXmlElement::NodeContainer& getContentContainer() const;

  /*!
  Set/Get parent node if any, NULL if none
  */
  AXmlElement *getParent() const { return mp_Parent; }
  void setParent(AXmlElement *pParent) { mp_Parent = pParent; }

  //a_Clone of self used in deep copy
  virtual AXmlElement* clone() const;

  /*!
  Parsing XML to and from file (assumes a perfect XML node without instructions, AXmlDocument accounts for instructions)
  */
  virtual void fromAFile(AFile&);
  virtual void toAFile(AFile&) const;

protected:
  //a_Name
  AString m_Name;

  //a_Content container
  NodeContainer m_Content;

  //a_Attributes
  AAttributes m_Attributes;

  //a_Parent node (NULL if none)
  AXmlElement *mp_Parent;

  //a_Recursive search function
  AXmlElement *_get(LIST_AString& xparts) const;

  //a_Indents with 2 spaces per indent
  inline void _indent(AOutputBuffer&, int) const;

  /*! Internal find */
  size_t _find(LIST_AString listPath, AXmlElement::ConstNodeContainer& result) const;

  /*! Add element */
  AXmlElement *_addElement(const AString& path, bool insert);

  /*! Creates a new element */
  AXmlElement *_createAndAppend(LIST_AString& xparts, AXmlElement* pParent);
  
  /*! Attempts to insert */
  AXmlElement *_getAndInsert(LIST_AString& xparts, AXmlElement* pParent);

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
