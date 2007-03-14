#include "pchABase.hpp"
#include "AContentType_Form.hpp"
#include "AHTTPResponseHeader.hpp"
#include "ARope.hpp"

const AString AContentType_Form::CONTENT_TYPE("application/x-www-form-urlencoded");

AContentType_Form::AContentType_Form()
{
}

void AContentType_Form::emit(AOutputBuffer& target, size_t /* indent = AConstant::npos */) const
{
  m_Form.emit(target);
}

void AContentType_Form::toAFile(AFile& aFile) const
{
  m_Form.toAFile(aFile);
}

size_t AContentType_Form::getContentLength(bool recalculate /* = true */)
{
  if (m_ContentLength == AConstant::npos || recalculate)
  {
    ARope rope;
    m_Form.emit(rope);
    m_ContentLength = rope.getSize();
  }

  return m_ContentLength;
}

void AContentType_Form::clear() 
{ 
  m_Form.clear();
}

void AContentType_Form::parse()
{ 
  m_Form.parse(mstr_Data); 
}
