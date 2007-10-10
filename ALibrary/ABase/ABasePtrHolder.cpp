#include "pchABase.hpp"

#include "ABasePtrHolder.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "ATextGenerator.hpp"
#include "AUrl.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void ABasePtrHolder::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ABasePtrHolder @ " << std::hex << this << std::dec << ") {" << std::endl;

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
#endif

ABasePtrHolder::ABasePtrHolder()
{
}

ABasePtrHolder::~ABasePtrHolder()
{
  try
  {
    m_BasePtrs.clear();  //a_Explicit
  }
  catch(...) {}
}

ABasePtrHolder::HOLDER& ABasePtrHolder::useContainer()
{ 
  return m_BasePtrs; 
}

void ABasePtrHolder::insert(
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

void ABasePtrHolder::setOwnership(
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

const ABase *ABasePtrHolder::get(const AString& name) const
{
  HOLDER::const_iterator cit = m_BasePtrs.find(name);
  if (cit != m_BasePtrs.end())
    return cit->second.get();
  else
    return NULL;
}

ABase *ABasePtrHolder::use(const AString& name)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
    return it->second.use();
  else
    return NULL;
}

void ABasePtrHolder::remove(const AString& name)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
    m_BasePtrs.erase(it);  //a_Remove from object base
  else
    ATHROW_EX(this, AException::NotFound, name);
}

void ABasePtrHolder::clear()
{
  m_BasePtrs.clear();
}

void ABasePtrHolder::emit(AOutputBuffer& target) const
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
