/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"

#include "ABasePtrContainer.hpp"
#include "AException.hpp"
#include "AXmlElement.hpp"

void ABasePtrContainer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_BasePtrs={" << std::endl;
  for(HOLDER::const_iterator cit = m_BasePtrs.begin(); cit != m_BasePtrs.end(); ++cit)
  {
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "={" << std::endl;
    (*cit).second->debugDump(os, indent+3);
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
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
    for (HOLDER::iterator it = m_BasePtrs.begin(); it != m_BasePtrs.end(); ++it)
      delete it->second;
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
  AASSERT(this, m_BasePtrs.size()<DEBUG_MAXSIZE_ABasePtrContainer);  //a_Debug only limit

  if (!pBase)
    ATHROW(this, AException::InvalidParameter);

  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
  {
    //a_Exists
    if (overwrite)
    {
      it->second->reset(pBase, ownership);  //a_Replace
    }
    else
      ATHROW(this, AException::ObjectContainerCollision);
  }
  else
  {
    ITEM *pitem = new ITEM(pBase, ownership);
    m_BasePtrs.insert(HOLDER::value_type(name, pitem));  //a_Insert new item
  }
}

void ABasePtrContainer::setOwnership(
  const AString& name,
  bool ownership  // = true
)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
    m_BasePtrs[name]->setOwnership(ownership);
  else
    ATHROW_EX(this, AException::NotFound, name);
}

const ABase *ABasePtrContainer::get(const AString& name) const
{
  HOLDER::const_iterator cit = m_BasePtrs.find(name);
  if (cit != m_BasePtrs.end())
    return cit->second->get();
  else
    return NULL;
}

ABase *ABasePtrContainer::use(const AString& name)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
  {
    ITEM *pitem = it->second;
    return pitem->use();
  }
  else
    return NULL;
}

void ABasePtrContainer::remove(const AString& name)
{
  HOLDER::iterator it = m_BasePtrs.find(name);
  if (it != m_BasePtrs.end())
  {
    delete it->second;     //a_Delete holder object first
    m_BasePtrs.erase(it);  //a_Remove holder from collection
  }
  else
    ATHROW_EX(this, AException::NotFound, name);
}

void ABasePtrContainer::clear()
{
  for (HOLDER::iterator it = m_BasePtrs.begin(); it != m_BasePtrs.end(); ++it)
    delete it->second;
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
    target.append(typeid(*((*cit).second)).name());
    target.append(" @ ",3);
    target.append(AString::fromPointer((*cit).second));

    ++cit;
    if (cit != m_BasePtrs.end())
      target.append(AConstant::ASTRING_CRLF);
  }
  target.append('}');
}

AXmlElement& ABasePtrContainer::emitXml(AXmlElement& thisRoot) const
{
  for (HOLDER::const_iterator cit = m_BasePtrs.begin(); cit != m_BasePtrs.end(); ++cit)
    thisRoot.addElement((*cit).first).addData(ASWNL(typeid((*cit).second).name()), AXmlElement::ENC_CDATADIRECT);

  return thisRoot;
}
