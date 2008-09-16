/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ANamePointerPair.hpp"
#include "AException.hpp"
#include "AOutputBuffer.hpp"

void ANamePointerPair::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "m_name='" << m_name.c_str() << "'";
  ADebugDumpable::indent(os, indent+1) << "mp_value='" << (u8)mp_value << "'" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ANamePointerPair::ANamePointerPair(const AString &name, void *pValue) :
  m_name(name), 
  mp_value(pValue)
{
}

ANamePointerPair::ANamePointerPair(const ANamePointerPair &nvSource) :
  m_name(nvSource.m_name),
  mp_value(nvSource.mp_value)
{
}

ANamePointerPair::~ANamePointerPair()
{
}

void ANamePointerPair::emit(AOutputBuffer& target) const
{
  target.append(m_name);
  target.append('=');
  target.append(AString::fromPointer(mp_value));
}

void ANamePointerPair::clear()
{  
  //a_Clear internal pair
  m_name.clear();
  mp_value = NULL;
}

void ANamePointerPair::setPointer(void *pValue)
{ 
  mp_value = pValue;
}

int ANamePointerPair::compare(const ANamePointerPair& nvSource) const
{
  int iRet = m_name.compare(nvSource.m_name);
  if (iRet)
    return iRet;

  //a_Pointers compared
  if (mp_value > nvSource.mp_value)
    return 1;
  else if (mp_value < nvSource.mp_value)
    return -1;

  return 0;
}

const AString& ANamePointerPair::getName() const
{ 
  return m_name;
}

AString& ANamePointerPair::useName()
{ 
  return m_name;
}

void *ANamePointerPair::getPointer() const
{ 
  return mp_value;
}

bool ANamePointerPair::isValid() const
{ 
  return (m_name.getSize() > 0);
}
