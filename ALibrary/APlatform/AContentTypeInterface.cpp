/*
Written by Alex Chachanashvili

$Id: AContentTypeInterface.cpp 259 2008-11-04 22:02:51Z achacha $
*/
#include "pchAPlatform.hpp"
#include "AContentTypeInterface.hpp"
#include "AFile.hpp"
#include "AHTTPHeader.hpp"

void AContentTypeInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Data={" << std::endl;
  m_Data.debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_ContentLength=" << m_ContentLength << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_ContentType=" << mstr_ContentType << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AContentTypeInterface::AContentTypeInterface(const AString& type) :
  m_ContentLength(AConstant::npos),
  mstr_ContentType(type)
{
}

AContentTypeInterface::~AContentTypeInterface()
{
}

void AContentTypeInterface::parseHTTPHeader(const AHTTPHeader& header)
{
  AString str;
  if (header.get(AHTTPHeader::HT_ENT_Content_Type, str))
  {
    if (str.compare(mstr_ContentType))
      ATHROW_EX(this, AException::InvalidData, ASWNL("content-type: ") + str + " found instead of " + mstr_ContentType);
  }
  else
  {
    //ATHROW_EX(this, AException::InvalidData, ASWNL("Content-Type: not found in the header"));
  }

  //a_For now only Content-Length matters
  //a_ -1 means it is unknown are read until EOF
  str.clear();
  if (header.get(AHTTPHeader::HT_ENT_Content_Length, str))
    m_ContentLength = str.toS4();
  else 
    m_ContentLength = AConstant::npos;
}

void AContentTypeInterface::toAFile(AFile& fOut) const
{
  m_Data.emit(fOut);
}

void AContentTypeInterface::fromAFile(AFile& fIn)
{
  m_Data.clear();
  if (!m_ContentLength)
    return;

  //a_Read in the data
  if (AConstant::npos != m_ContentLength)
  {
    fIn.read(m_Data, m_ContentLength);
  }
  else
  {
    fIn.readUntilEOF(m_Data);
    m_ContentLength = m_Data.useAString().getSize();
  }
  
  if (m_Data.useAString().getSize() > 0)
  {
    //a_Call the needed parse (derived classes override this)
    parse();
  }
}

AFile& AContentTypeInterface::useData()
{ 
  return m_Data;
}

void AContentTypeInterface::emit(AOutputBuffer& target) const
{
  m_Data.emit(target);
}

void AContentTypeInterface::clear()
{
  m_Data.useAString().clear(true); 
}

const AString& AContentTypeInterface::getContentType() const 
{ 
  return mstr_ContentType; 
}

void AContentTypeInterface::setContentType(const AString& strType) 
{ 
  mstr_ContentType = strType; 
}

size_t AContentTypeInterface::getContentLength() 
{ 
  return (m_ContentLength == AConstant::npos ? 0 : m_ContentLength); 
}
