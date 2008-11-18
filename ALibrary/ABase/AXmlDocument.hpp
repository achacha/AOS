/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlDocument_HPP__
#define INCLUDED__AXmlDocument_HPP__

#include "apiABase.hpp"
#include "AXmlElement.hpp"
#include "ASerializable.hpp"
#include "AString.hpp"
#include "AXmlData.hpp"
#include "AXmlInstruction.hpp"
#include "ADebugDumpable.hpp"
#include "AJsonEmittable.hpp"
#include "AXmlEmittable.hpp"

/*!
Wrapper to an XML document

To access the XML use useRoot() call to get the root AXmlElement object
*/
class ABASE_API AXmlDocument : public AXmlEmittable, public ASerializable, public AJsonEmittable, public ADebugDumpable
{
public:
  //! Default name for the root element inthe document: root
  static const AString DEFAULT_ROOT;

public:
  /*!
  ctor
  Construct element with a root element
  Document owns and will delete this object

  @param rootName of the root element
  @param pXmlHeader is a new XML_HEADER instruction that is different from <?xml version="1.0" encoding="UTF-8"?>
  */
  AXmlDocument(const AString& rootName = AXmlDocument::DEFAULT_ROOT, AXmlInstruction *pXmlHeader = NULL);

  //! ctor from AFile
  AXmlDocument(AFile&);
  
  //! copy ctor
  AXmlDocument(const AXmlDocument&);
  
  //! dtor
  virtual ~AXmlDocument();
  
  /*!
  XML document building
  Adds instructions to the header
  During emit, instructions of the document come first, in the order added
  By default <?xml ?> is added upon creation
  
  @param type of instruction to add to header
  */
  AXmlInstruction& addInstruction(AXmlInstruction::TYPE type);
  
  /*!
  Add comment

  @param comment to add
  */
  AXmlInstruction& addComment(const AString& comment);

  /*!
  Access to the document root AXmlElement
  This is the main access point to the document data
  Modifications/additions/inserts are all done via this element

  @return reference to the root AXmlElement
  @see AXmlElement
  */
  AXmlElement& useRoot();

  /*!
  Root element read-only
  
  @return constant reference root AXmlElement
  @see AXmlElement
  */
  const AXmlElement& getRoot() const;
  
  /*!
  AEmittable
  
  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AEmittable
  

  @param target to append to
  @param indent -1=none, >=0 will add 2 spaces per indent and make it human-readable (use -1 for to reduce spaces/CRLF, XSLT/etc parsers don't care about formatting)
  */
  virtual void emit(AOutputBuffer& target, int indent) const;
  
  /*!
  Emits XML nodes in this document
  Does NOT emit the instructions (use toAFile to output full document)
  
  @param thisRoot to append root and contents to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  AJsonEmittable
  Emit JSON format

  @param target to emit JSONO object to
  @param indent -1=none, >=0 for formatted output (2 spaces per indent level)
  */
  virtual void emitJson(AOutputBuffer& target, int indent = -1) const;

  /*! 
  Only clears root element and retains instructions, assigns new rootName is not empty else keeps old one
  Since a document requires a root name the only way to clear it is to useRoot().useName().clear() - not recommended
  
  @param rootName if non-empty will set it as the new name, else will keep the existing name
  @param keepInstructions by default instructions added to the document are not cleared
  */
  void clear(const AString& rootName = AConstant::ASTRING_EMPTY, bool keepInstructions = false);

  /*!
  ASerializable
  @param aFile to read from
  */
  virtual void fromAFile(AFile& aFile);

  /*!
  ASerializable
  @param aFile to write to
  */
  virtual void toAFile(AFile& aFile) const;

  /*!
  Clone the document

  @return cloned object
  */
  virtual AXmlDocument* clone() const;

private:
  //a_Instruction elements order sensitive and go before the xml data
  typedef std::list<AXmlInstruction *> LIST_NODEPTR;
  LIST_NODEPTR m_Instructions;

  //a_There is only 1 root element
  AXmlElement m_Root;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AXmlDocument_HPP__
