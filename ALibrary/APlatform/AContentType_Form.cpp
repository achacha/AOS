/*
Written by Alex Chachanashvili

$Id: AContentType_Form.cpp 205 2008-05-29 13:58:16Z achacha $
*/
#include "pchAPlatform.hpp"
#include "AContentType_Form.hpp"
#include "ARope.hpp"

const AString AContentType_Form::CONTENT_TYPE("application/x-www-form-urlencoded");

void AContentType_Form::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  AContentTypeInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent+1) << "m_Form={" << std::endl;
  m_Form.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AContentType_Form::AContentType_Form() :
  AContentTypeInterface(CONTENT_TYPE)
{
}

void AContentType_Form::emit(AOutputBuffer& target) const
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
  AContentTypeInterface::clear();
}

void AContentType_Form::parse()
{ 
  m_Form.parse(m_Data); 
}

AQueryString& AContentType_Form::useForm()
{ 
  return m_Form;
}
