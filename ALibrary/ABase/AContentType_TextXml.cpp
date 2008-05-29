/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AContentType_TextXml.hpp"

const AString AContentType_TextXml::CONTENT_TYPE("text/xml",8);  // text/xml

void AContentType_TextXml::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  AContentTypeInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent+1) << "m_Document={" << std::endl;
  m_Document.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m__boolFormatted=" << m__boolFormatted << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__boolNeedParsing=" << m__boolNeedParsing << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AContentType_TextXml::AContentType_TextXml() :
  AContentTypeInterface(CONTENT_TYPE),
  m__boolFormatted(false),
  m__boolNeedParsing(true),
  m_Document(ASW("root",4))
{ 
}

AContentType_TextXml::~AContentType_TextXml()
{ 
}

void AContentType_TextXml::parse()
{
  if (!m_Data.isNotEof())
    ATHROW(this, AException::ProgrammingError);

  m_Document.fromAFile(m_Data);
  m__boolNeedParsing = false;
}

void AContentType_TextXml::emit(AOutputBuffer& target) const
{
  m_Document.emit(target, (m__boolFormatted || !m__boolNeedParsing ? 0 : AConstant::npos));
}

AXmlDocument& AContentType_TextXml::useDocument()
{
  return m_Document;
}

void AContentType_TextXml::setFormatted(bool boolFlag) 
{ 
  m__boolFormatted = boolFlag;
}

void AContentType_TextXml::clear()
{
  //a_Cleanup
  m_Document.clear();
  AContentTypeInterface::clear();
}

void AContentType_TextXml::toAFile(AFile& aFile) const
{
  //a_Parse is needed before write can occur
  if (m__boolNeedParsing)
    ATHROW(this, AException::ProgrammingError);

  m_Document.toAFile(aFile);
}

void AContentType_TextXml::fromAFile(AFile& aFile)
{
  m_Document.fromAFile(aFile);

  //a_Set the flag to false, parent calls the virtual parse() method by default
  m__boolNeedParsing = false;
}
