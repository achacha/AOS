#include "pchABase.hpp"
#include "AContentTypeInterface.hpp"
#include "AFile.hpp"
#include "AException.hpp"
#include "AUrl.hpp"
#include "AHTTPHeader.hpp"

#ifdef __DEBUG_DUMP__
void AContentTypeInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AContentTypeInterface @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ContentLength=" << m_ContentLength << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_ContentType=" << mstr_ContentType << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AContentTypeInterface::AContentTypeInterface() :
  m_ContentLength(AConstant::npos)
{
}

AContentTypeInterface::AContentTypeInterface(const AString& type) :
  m_ContentLength(AConstant::npos),
  mstr_ContentType(type)
{
}

AContentTypeInterface::~AContentTypeInterface()
{
}

void AContentTypeInterface::parseHTTPHeader(AHTTPHeader& header)
{
  AString str;
  if (header.getPairValue(AHTTPHeader::HT_ENT_Content_Type, str))
  {
    if (str.compare(mstr_ContentType))
      ATHROW_EX(this, AException::InvalidData, ASWNL("Content-Type: ") + str + " found instead of " + mstr_ContentType);
  }
  else
  {
    //ATHROW_EX(this, AException::InvalidData, ASWNL("Content-Type: not found in the header"));
  }

  //a_For now only Content-Length matters
  //a_ -1 means it is unknown are read until EOF
  str.clear();
  if (header.getPairValue(AHTTPHeader::HT_ENT_Content_Length, str))
    m_ContentLength = str.toS4();
  else 
    m_ContentLength = AConstant::npos;
}

void AContentTypeInterface::toAFile(AFile& fOut) const
{
  fOut.write(mstr_Data);
}

void AContentTypeInterface::fromAFile(AFile& fIn)
{
  mstr_Data.clear();
  if (!m_ContentLength)
    return;

  //a_Read in the data
  if (AConstant::npos != m_ContentLength)
  {
    fIn.read(mstr_Data, m_ContentLength);
  }
  else
  {
    fIn.readUntilEOF(mstr_Data);
    m_ContentLength = mstr_Data.getSize();
  }
  
  if (mstr_Data.getSize() > 0)
  {
    //a_Call the needed parse (derived classes override this)
    parse();
  }
}

void AContentTypeInterface::setData(const AString& strData)
{ 
  mstr_Data.assign(strData);
}

void AContentTypeInterface::addData(const AString& strData)
{ 
  mstr_Data.append(strData);
}

AString& AContentTypeInterface::useData()
{ 
  return mstr_Data;
}

void AContentTypeInterface::emit(AOutputBuffer& target, size_t /* indent = AConstant::npos */) const
{
  target.append(mstr_Data);
}

void AContentTypeInterface::clear()
{
  mstr_Data.clear(true); 
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
