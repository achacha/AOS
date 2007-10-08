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

  MAP_ASTRING_ABASE::const_iterator cit = m_BasePtrs.begin();
  while (cit != m_BasePtrs.end())
  {
    ADebugDumpable::indent(os, indent+1) << 
      (*cit).first << 
      "=[" << 
      typeid(*((*cit).second)).name() << 
      "]:0x" << 
      std::hex << AString::fromPointer((*cit).second) << std::dec << 
      std::endl;
    ++cit;
  }

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ABasePtrHolder::ABasePtrHolder()
{
}

ABasePtrHolder::ABasePtrHolder(const ABasePtrHolder& that)
{
  MAP_ASTRING_ABASE::const_iterator cit = that.m_BasePtrs.begin();
  while (cit != that.m_BasePtrs.end())
  {
    m_BasePtrs[(*cit).first] = (*cit).second;
    ++cit;
  }
}

ABasePtrHolder::~ABasePtrHolder()
{
  try
  {
    //a_Release owned pointers
    for (MAP_ABASE_PTRS::iterator it = m_OwnedPtrs.begin(); it != m_OwnedPtrs.end(); ++it)
    {
      delete it->first;
    }
  }
  catch(...) {}

}

void ABasePtrHolder::insert(const AString& name, ABase *pBase, bool overwrite)
{
  if (!pBase)
    ATHROW(this, AException::InvalidParameter);

  if (!overwrite)
  {
    if (m_BasePtrs.find(name) != m_BasePtrs.end())
      ATHROW(this, AException::ObjectContainerCollision);
  }

  m_BasePtrs[name] = pBase;
}

void ABasePtrHolder::insertWithOwnership(const AString& name, ABase *pBase, bool overwrite)
{
  if (!pBase)
    ATHROW(this, AException::InvalidParameter);

  if (!overwrite)
  {
    if (m_BasePtrs.find(name) != m_BasePtrs.end())
      ATHROW(this, AException::ObjectContainerCollision);
  }
  else
    AASSERT(this, m_OwnedPtrs.end() == m_OwnedPtrs.find(pBase));

  m_BasePtrs[name] = pBase;
  m_OwnedPtrs[pBase] = true;
}

ABase *ABasePtrHolder::get(const AString& name) const
{
  MAP_ASTRING_ABASE::const_iterator cit = m_BasePtrs.find(name);
  if (cit != m_BasePtrs.end())
    return cit->second;
  else
    return NULL;
}

void ABasePtrHolder::remove(const AString& name)
{
  MAP_ASTRING_ABASE::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
  {
    //a_Remove from ownership and delete if owned
    MAP_ABASE_PTRS::iterator itOwned = m_OwnedPtrs.find(it->second);
    if (itOwned != m_OwnedPtrs.end())
    {
      if (itOwned->second)
        delete itOwned->first;
      m_OwnedPtrs.erase(itOwned);
    }

    //a_Remove from object base
    m_BasePtrs.erase(it);
  }
}

void ABasePtrHolder::clear()
{
  m_BasePtrs.clear();
}

void ABasePtrHolder::emit(AOutputBuffer& target) const
{
//TODO:
#pragma message("ABasePtrHolder::emit: TODO")
}
