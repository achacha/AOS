#ifndef INCLUDED__AXmlDocument_HPP__
#define INCLUDED__AXmlDocument_HPP__

#include "apiABase.hpp"
#include "AXmlNode.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AXmlInstruction.hpp"

/*!
Wrapper to an XML document

To access the XML use useRoot() call to get the root AXmlElement object
*/
class ABASE_API AXmlDocument : public ADebugDumpable, public ASerializable
{
public:
  /*!
  Construct element with a root element
  pXmlHeader is a new XML_HEADER instruction that is different from <?xml version="1.0" encoding="UTF-8"?>
  Document owns and will delete this object
  */
  AXmlDocument(const AString& rootName, AXmlInstruction *pXmlHeader = NULL);
  AXmlDocument(AFile&);
  virtual ~AXmlDocument();
  
  /*!
  XML document building
  Adds instructions to the header
  During emit, instructions of the document come first, in the order added
  By default <?xml ?> is added upon creation
  */
  AXmlInstruction& addInstruction(AXmlInstruction::TYPE type);
  AXmlInstruction& addComment(const AString& comment);

  /*!
  Access to the document root AXmlElement
  This is the main access point to the document data
  Modifications/additions/inserts are all done via this element
  */
  AXmlElement& useRoot();

  /*!
  Root element read-only
  */
  const AXmlElement& getRoot() const;

  /*!
  TODO: High level wrappers for getting,setting,testing,adding elements and cdata
  */
  
  /*!
  Use -1 for all cases processing cases to reduce spaces/CRLF, XSLT/etc parsers don't care about formatting
  indent >=0 will add 2 spaces per indent and make it human-readable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer&, int indent) const;

  /*!
  Emit JSON format
  */
  virtual void emitJSON(AOutputBuffer&, int indent = -1) const;

  /*! 
  clearAll - clears root element name, data and instructions (must manually re-add XML_HEADER and set root element name)
  clear - only clears root element and retains the root name and instructions
  */
  void clearAll();
  void clear();

  /*!
  Parsing XML to and from file
  */
  virtual void fromAFile(AFile&);
  virtual void toAFile(AFile&) const;
  
private:
  AXmlDocument() {}  //a_No default dtor, must have root element name

  //a_Instruction elements order sensitive and go before the xml data
  typedef std::list<AXmlInstruction *> LIST_NODEPTR;
  LIST_NODEPTR m_Instructions;

  //a_There is only 1 root element
  AXmlElement m_Root;

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

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AXmlDocument_HPP__
