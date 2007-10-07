#include "pchABase.hpp"

#include "AObjectPtrHolder.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "ATextGenerator.hpp"
#include "AUrl.hpp"

#ifdef __DEBUG_DUMP__
void AObjectPtrHolder::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AObjectPtrHolder[\"" << m_Name << "\"] @ " << std::hex << this << std::dec << ") {" << std::endl;

  MAP_ASTRING_AOBJECTBASE::const_iterator cit = m_ObjectPointers.begin();
  while (cit != m_ObjectPointers.end())
  {
    ADebugDumpable::indent(os, indent+1) << (*cit).first << "=0x" << std::hex << AString::fromPointer((*cit).second) << std::dec << std::endl;;
    ++cit;
  }

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AObjectPtrHolder::AObjectPtrHolder(const AString& name /* = AConstant::ASTRING_EMPTY */) :
  AObjectBase(name)
{
}

AObjectPtrHolder::AObjectPtrHolder(const AObjectPtrHolder& that) :
  AObjectBase(that)
{
  MAP_ASTRING_AOBJECTBASE::const_iterator cit = that.m_ObjectPointers.begin();
  while (cit != that.m_ObjectPointers.end())
  {
    m_ObjectPointers[(*cit).first] = (*cit).second;
    ++cit;
  }
}

AObjectPtrHolder::~AObjectPtrHolder()
{
  //a_Do nothing, the pointers are NOT owned and will not be deleted
}

void AObjectPtrHolder::insert(const AString& name, AObjectBase *pobject, bool overwrite)
{
  if (!pobject)
    ATHROW(this, AException::InvalidParameter);

  if (!overwrite)
  {
    if (m_ObjectPointers.find(name) != m_ObjectPointers.end())
      ATHROW(this, AException::ObjectContainerCollision);
  }

  m_ObjectPointers[name] = pobject;
}

AObjectBase *AObjectPtrHolder::get(const AString& name) const
{
  MAP_ASTRING_AOBJECTBASE::const_iterator cit = m_ObjectPointers.find(name);
  if (cit != m_ObjectPointers.end())
    return cit->second;
  else
    return NULL;
}

void AObjectPtrHolder::remove(const AString& path)
{
  MAP_ASTRING_AOBJECTBASE::iterator it = m_ObjectPointers.find(path);
  if (it != m_ObjectPointers.end())
  {
    m_ObjectPointers.erase(it);
  }
}

void AObjectPtrHolder::clear()
{
  m_ObjectPointers.clear();
}

void AObjectPtrHolder::emit(AOutputBuffer& target) const
{
//TODO:
#pragma message("AObjectPtrHolder::emit: TODO")
}

void AObjectPtrHolder::emitXml(AXmlElement& target) const
{
//TODO:
#pragma message("AObjectPtrHolder::emitXml: TODO")
}
