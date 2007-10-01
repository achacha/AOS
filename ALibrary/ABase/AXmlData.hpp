#ifndef INCLUDED__AXmlData_HPP__
#define INCLUDED__AXmlData_HPP__

#include "apiABase.hpp"
#include "AXmlNode.hpp"
#include "AString.hpp"

class ABASE_API AXmlData : public AXmlNode
{
public:
  //a_The way the data will be encoded
  enum Encoding
  {
    None = 0,     //a_Text as is
    Url,          //a_Url encoded text
    CDataSafe,    //a_<![CDATA[ data ]]> with data made safe for CData
    CDataDirect,  //a_<![CDATA[ data ]]> with data as is
    XmlSafe,      //a_Replaces < > &  with &lt; &gt; &amp;
    Base64,       //a_Encodes with Base64
    CDataHexDump  //a_Debugging format useful for examining binary data
  };

public:
  AXmlData();
  AXmlData(const AXmlData&);
  AXmlData(const AEmittable& data, AXmlData::Encoding encoding = AXmlData::None, AXmlNode *pParent = NULL);
  virtual ~AXmlData();
  
  /*!
  Change encoding of data
  */
  void setEncoding(AXmlData::Encoding);

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
  virtual void emitJSON(AOutputBuffer&, int indent = -1) const;
  
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
  virtual AXmlNode* clone() const { return new AXmlData(*this); }

protected:
  AString m_Data;
  Encoding m_Encoding;

public:
  static const AString sstr_StartCDATA;  //a_"<![CDATA["
  static const AString sstr_EndCDATA;    //a_"]]>"

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif
