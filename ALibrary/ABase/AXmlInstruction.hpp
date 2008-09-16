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
  //! Enums for the most common types
  enum TYPE
  {
    XML_HEADER,
    XML_STYLESHEET,
    DOCTYPE,
    DTD_ELEMENT,
    COMMENT
  };
  
public:
  //! ctor
  AXmlInstruction();
  
  /*!
  Create instruction by type

  @param type of the instruction
  @param pParent element
  */
  AXmlInstruction(const AXmlInstruction::TYPE type, AXmlElement *pParent = NULL);

  /*!
  Create instruction by name

  @param name of the instruction
  @param pParent element
  */
  AXmlInstruction(const AString& name, AXmlElement *pParent = NULL);

  /*!
  Create instruction by type with attributes and data

  @param type of the instruction
  @param attrs attributes to set on this instruction
  @param data associated to the instruction
  @param pParent element
  */
  AXmlInstruction(const AXmlInstruction::TYPE type, const AXmlAttributes& attrs, const AString& data = AConstant::ASTRING_EMPTY, AXmlElement *pParent = NULL);
  
  //! copy ctor
  AXmlInstruction(const AXmlInstruction&);
  
  //! dtor
  virtual ~AXmlInstruction();

  /*!
  Parse contents of the instruction

  @param source to parse
  */
  void parse(const AString& source);
  
  /*!
  Type of this instruction
  
  @return instruction type
  */
  AXmlInstruction::TYPE getType() const;

  /*!
  Set the type

  @param type of instruction
  */
  void setType(AXmlInstruction::TYPE type);

  /*!
  Returns the terminator for parsing of the current instruction type
  e.g. XML_HEADER is "?>"
  
  @return terminator type for this instruction
  */
  const AString& getTypeTerminator();

  /*!
  Access to data

  @return reference to the data
  */
  AString& useData();

  /*!
  AXmlEmittable
  
  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  Emit self with indent
  
  @param target to append to
  @param indent -1=none, >=0 for indent, each indent is 2 spaces
  */
  virtual void emit(AOutputBuffer& target, int indent) const;

  /*!
  AJsonEmittable
  Only instruction that JSON can handle is a comment, others don't map

  @param target to append JSON to
  @param indent -1=none, >=0 for indent, each indent is 2 spaces
  */
  virtual void emitJson(AOutputBuffer& target, int indent = -1) const;

  /*!
  Checks if element type

  @return false for this type
  @see AXmlElement
  */
  virtual bool isElement() const;

  /*!
  Checks if data type

  @return false for this type
  @see AXmlElement
  @see AXmlData
  */
  virtual bool isData() const;

  /*!
  Checks if instruction

  @return true for this type
  @see AXmlElement
  */
  virtual bool isInstruction() const;

  /*!
  Clear
  NOTE: Does not clear the name (i.e. the type of this instruction)
  */
  virtual void clear();

  /*!
  Clone of self used in deep copy

  @return new cloned object
  */
  virtual AXmlElement* clone() const;

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
