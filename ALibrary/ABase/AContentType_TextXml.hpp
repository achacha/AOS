/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AContentType_TextXml_HPP__
#define INCLUDED__AContentType_TextXml_HPP__

#include "apiABase.hpp"
#include "AContentTypeInterface.hpp"
#include "AXmlDocument.hpp"

class AOutputBuffer;

class ABASE_API AContentType_TextXml : public AContentTypeInterface
{
public:
  static const AString CONTENT_TYPE;  // text/xml

public:
  AContentType_TextXml();
  virtual ~AContentType_TextXml();

  /*!
  Clear data and xml
  */
  virtual void clear();

  /*!
  Activation methods do nothing, binary data stores all types and knows nothing about them
  */
  virtual void parse();

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;  

  /*!
  Access to xml document
  */
  AXmlDocument& useDocument();

  /*!
  Sets formatting of the output
  */
  void setFormatted(bool boolFlag = true);

  /*!
  ASerializable
  */
  virtual void fromAFile(AFile& fileIn);
  virtual void toAFile(AFile& fileIn) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AXmlDocument m_Document;

  bool m__boolFormatted;

  //a_Parsing flag
  bool m__boolNeedParsing;
};

#endif

