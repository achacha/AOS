/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlInstruction_HPP__
#define INCLUDED__AXmlInstruction_HPP__

#include "apiABase.hpp"
#include "AXmlElement.hpp"
#include "AXmlAttributes.hpp"

class AOutputBuffer;

/*!
  XML Instruction element
  <?xml $attributes $data ?>   (NOTE: data should be empty, but if it is not it better be valid)
  <?xml-stylesheet $atributes $data ?>
  <!DOCTYPE $attributes $data >
  <!ELEMENT $attributes $data >
  <?$name $attributes $data >  (user_instruction can be anything not already reserved)
  <!-- $attributes $data -->

  Name of the element MUST include the leading symbol:
    ?xml
    !DOCTYPE
    !ELEMENT
    !--
    etc...

Adding a comment to XML element (AXmlElement::addComment() does this):
  AXmlElement myElement;
  AXmlInstruction *p = AXmlInstruction(AXmlInstruction::COMMENT);
  p->useData().assign("This is my comment");
  myElement.addContent(p);
*/
class ABASE_API AXmlInstruction : public AXmlElement
{
public:
  //a_Enums for the most common types
  enum TYPE
  {
    XML_HEADER,
    XML_STYLESHEET,
    DOCTYPE,
    DTD_ELEMENT,
    COMMENT
  };
  
public:
  /*!
  ctors
  */
  AXmlInstruction() {}
  AXmlInstruction(const AXmlInstruction::TYPE type, AXmlElement *pParent = NULL);
  AXmlInstruction(const AString& name, AXmlElement *pParent = NULL);
  AXmlInstruction(const AXmlInstruction::TYPE type, const AXmlAttributes& attrs, const AString& data = AConstant::ASTRING_EMPTY, AXmlElement *pParent = NULL);
  AXmlInstruction(const AXmlInstruction&);
  virtual ~AXmlInstruction() {}

  /*!
  Parse contents of the instruction
  */
  void parse(const AString&);
  
  /*!
  Type of this instruction
  */
  AXmlInstruction::TYPE getType() const;

  /*!
  Set the type
  */
  void setType(AXmlInstruction::TYPE type);

  /*!
  Returns the terminator for parsing for the current type
  e.g. XML_HEADER is "?>"
  */
  const AString& getTypeTerminator();

  /*!
  Access to $data
  */
  AString& useData() { return m_Data; }

  /*!
  Emit self
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer&, int indent) const;

  /*!
  AJsonEmittable
  */
  virtual void emitJson(AOutputBuffer&, int indent = -1) const;

  /*!
  Checks if element type
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
  Clear
  NOTE: Does not clear the name (i.e. the type of this instruction)
  */
  virtual void clear();

  /*!
  Clone of self used in deep copy
  */
  virtual AXmlElement* clone() const { return new AXmlInstruction(*this); }

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  // Parsing constants
  static const AString S_DEFAULT;
  static const AString S_XML;
  static const AString S_COMMENT;

  // Format <$start$name $attributes $data $end>
  AString m_Data;                //a_DOCTYPE and comment will use this and store contents as is
  TYPE m_Type;                   //a_Type of an instruction
};

#endif //__AXmlInstruction_HPP__
