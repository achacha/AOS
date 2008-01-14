#ifndef INCLUDED__AXmlData_HPP__
#define INCLUDED__AXmlData_HPP__

#include "apiABase.hpp"
#include "AXmlElement.hpp"
#include "AString.hpp"

class ABASE_API AXmlData : public AXmlElement
{
public:
  AXmlData();
  AXmlData(const AXmlData&);
  AXmlData(const AEmittable& data, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE, AXmlElement *pParent = NULL);
  virtual ~AXmlData();
  
  /*!
  Change encoding of data
  */
  void setEncoding(AXmlElement::Encoding);

  /*!
  Checks if there is any data
  */
  bool isEmpty() const { return m_Data.isEmpty(); }

  /*!
  Clear contents
  */
  virtual void clear();

  /*!
  Emit data and encode if needed
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer&, int indent) const;
  
  /*!
  AJsonEmittable
  */
  virtual void emitJson(AOutputBuffer&, int indent = -1) const;
  
  /*!
  Unencoded raw data emit
  */
  virtual void emitContent(AOutputBuffer&) const;
  
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
  Data access
  */
  AString& useData();
  const AString& getData() const;

  /*!
  Clone of self used in deep copy
  */
  virtual AXmlElement* clone() const { return new AXmlData(*this); }

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
