#include "pchABase.hpp"
#include "AElementInterface.hpp"

#ifdef __DEBUG_DUMP__
void AElementInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__boolSingular=" << (m__boolSingular ? AString::sstr_True : AString::sstr_False) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__pelementParent=0x" << (void *)m__pelementParent << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AElementInterface::AElementInterface(AElementInterface *pelementParent) :
  m__pelementParent(pelementParent),
  m__boolSingular(true)
{
}

void AElementInterface::reset()
{
  // Does not reset the parent pointer
	m__boolSingular = true;
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

