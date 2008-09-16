/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlData_HPP__
#define INCLUDED__AXmlData_HPP__

#include "apiABase.hpp"
#include "AXmlElement.hpp"
#include "AString.hpp"

class ABASE_API AXmlData : public AXmlElement
{
public:
  //! ctor
  AXmlData();
  
  //! copy ctor
  AXmlData(const AXmlData&);
  
  /*!
  ctor

  @param data from AEmittable
  @param encoding to use on data
  @param pParent parent element
  */
  AXmlData(const AEmittable& data, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, AXmlElement *pParent = NULL);
  
  //! dtor
  virtual ~AXmlData();
  
  /*!
  Change encoding of data

  @param encoding to use
  */
  void setEncoding(AXmlElement::Encoding encoding);

  /*!
  Checks if there is any data

  @return true if empty
  */
  bool isEmpty() const;

  /*!
  Clear contents
  */
  virtual void clear();

  /*!
  Emit data and encode if needed
  
  @param target to emit to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  Emit data and encode if needed
  
  @param target to emit to
  @param indent to use -1=none, >=0 will format and indent, each indent is two spaces
  */
  virtual void emit(AOutputBuffer& target, int indent) const;
  
  /*!
  AJsonEmittable

  @param target to emit JSON data to
  @param indent to use -1=none, >=0 will format and indent, each indent is two spaces
  */
  virtual void emitJson(AOutputBuffer& target, int indent = -1) const;
  
  /*!
  Unencoded raw data emit

  @param target to emit content to
  */
  virtual void emitContent(AOutputBuffer& target) const;
  
  /*!
  Checks if element type

  @return false for this class
  @see AXmlElement
  */
  virtual bool isElement() const;

  /*!
  Checks if data type

  @return true for this class
  @see AXmlElement
  */
  virtual bool isData() const;

  /*!
  Checks if instruction

  @return false for this class
  @see AXmlElement
  @see AXmlInstruction
  */
  virtual bool isInstruction() const;

  /*!
  Use the data

  @return reference to AString object that holds the data
  */
  AString& useData();

  /*!
  Get the data reference

  @return constant reference to AString object that holds the data
  */
  const AString& getData() const;

  /*!
  Clone of self used in deep copy

  @return clone of this object
  */
  virtual AXmlElement* clone() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  AString m_Data;
  Encoding m_Encoding;

public:
  static const AString sstr_StartCDATA;  //a_"<![CDATA["
  static const AString sstr_EndCDATA;    //a_"]]>"
};

#endif
