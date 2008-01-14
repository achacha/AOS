#include "pchABase.hpp"

#include "ABasePtrContainer.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "ATextGenerator.hpp"
#include "AUrl.hpp"
#include "AXmlElement.hpp"

void ABasePtrContainer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_BasePtrs={" << std::endl;
  HOLDER::const_iterator cit = m_BasePtrs.begin();
  while (cit != m_BasePtrs.end())
  {
    ADebugDumpable::indent(os, indent+2) << 
      (*cit).first << 
      "=[" << 
      typeid(*((*cit).second)).name() << 
      "]:";
    cit->second.debugDump(os, 0);
    os << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ABasePtrContainer::ABasePtrContainer()
{
}

ABasePtrContainer::~ABasePtrContainer()
{
  try
  {
    m_BasePtrs.clear();  //a_Explicit
  }
  catch(...) {}
}

ABasePtrContainer::HOLDER& ABasePtrContainer::useContainer()
{ 
  return m_BasePtrs; 
}

void ABasePtrContainer::insert(
  const AString& name, 
  ABase *pBase, 
  bool ownership, // = false
  bool overwrite  // = true
)
{
  if (!pBase)
    ATHROW(this, AException::InvalidParameter);

  if (m_BasePtrs.find(name) != m_BasePtrs.end())
  {
    //a_Exists
    if (overwrite)
      m_BasePtrs[name].reset(pBase, ownership);  //a_Replace
    else
      ATHROW(this, AException::ObjectContainerCollision);
  }
  else
    m_BasePtrs[name].reset(pBase, ownership);  //a_Insert new item
}

void ABasePtrContainer::setOwnership(
  const AString& name, 
  bool ownership  // = true
)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
    m_BasePtrs[name].setOwnership(ownership);
  else
    ATHROW_EX(this, AException::NotFound, name);
}

const ABase *ABasePtrContainer::get(const AString& name) const
{
  HOLDER::const_iterator cit = m_BasePtrs.find(name);
  if (cit != m_BasePtrs.end())
    return cit->second.get();
  else
    return NULL;
}

ABase *ABasePtrContainer::use(const AString& name)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
    return it->second.use();
  else
    return NULL;
}

void ABasePtrContainer::remove(const AString& name)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
    m_BasePtrs.erase(it);  //a_Remove from object base
  else
    ATHROW_EX(this, AException::NotFound, name);
}

void ABasePtrContainer::clear()
{
  m_BasePtrs.clear();
}

void ABasePtrContainer::emit(AOutputBuffer& target) const
{
  target.append('{');
  HOLDER::const_iterator cit = m_BasePtrs.begin();
  while (cit != m_BasePtrs.end())
  {
    target.append((*cit).first);
    target.append('=');
    target.append(AString::fromPointer((*cit).second));
    ++cit;
    if (cit != m_BasePtrs.end())
      target.append(',');
  }
  target.append('}');
}
