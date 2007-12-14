#include "pchABase.hpp"
#include "AElementInterface.hpp"

#ifdef __DEBUG_DUMP__
void AElementInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_strType=" << m_strType;
  ADebugDumpable::indent(os, indent+1) << "  m_isSingular=" << AString::fromBool(m_isSingular);
  ADebugDumpable::indent(os, indent+1) << "  mp_Parent=" << AString::fromPointer(mp_Parent) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AElementInterface::AElementInterface(AElementInterface *pelementParent, bool isSingular) :
  mp_Parent(pelementParent),
  m_isSingular(isSingular)
{
}

AElementInterface::~AElementInterface()
{
}

void AElementInterface::clear()
{
  // Does not reset the parent pointer
	m_isSingular = true;
	m_strType.clear();
}

bool AElementInterface::_isChild(AElementInterface *peChild)
{
  //a_Child is to a parent as parent is to child from iteration point of view
  //a_Start with child and iterate up the parent tree to check relationship
  AElementInterface *pParent = peChild;
  while(pParent != NULL)
  {
    if (pParent == this)
      return true;
  
    pParent = pParent->getParent();
  }
  return false;
}

bool AElementInterface::isWhiteSpace(char cX) const 
{ 
  return ((AConstant::ASTRING_WHITESPACE.find(cX) == AConstant::npos) ? false : true);
}

bool AElementInterface::isSingular() const 
{ 
  return m_isSingular; 
}

void AElementInterface::setSingular(bool boolFlag) 
{ 
  m_isSingular = boolFlag; 
}

const AString& AElementInterface::getType() const 
{ 
  return m_strType; 
}

void AElementInterface::setType(const AString& strType)
{
  m_strType = strType; m_strType.makeLower();
}

bool AElementInterface::isType(const AString& strType) 
{ 
  return (m_strType.compareNoCase(strType) ? false : true);
}

AElementInterface *AElementInterface::getParent() const
{ 
  return mp_Parent; 
}
