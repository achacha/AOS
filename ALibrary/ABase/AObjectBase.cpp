
#include "pchABase.hpp"

#include "AObjectBase.hpp"
#include "AException.hpp"
#include "AXmlElement.hpp"
#include "AFile.hpp"

#ifdef __DEBUG_DUMP__
void AObjectBase::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AObjectBase[\"" << m_Name << "\"] @ " << std::hex << this << std::dec << ")" << std::endl;
}
#endif

AObjectBase::AObjectBase() :
  mp_SynchObject(NULL)
{
}

AObjectBase::AObjectBase(const AString& name /* = AString::sstr_Empty */) :
  mp_SynchObject(NULL),
  m_Name(name)
{
}

AObjectBase::AObjectBase(const AObjectBase& that) :
  mp_SynchObject(NULL),      //NOTE: Synchronization is not copied
  m_Name(that.m_Name)
{
}

AObjectBase::~AObjectBase()
{
  delete mp_SynchObject;
}

void AObjectBase::setSynchronization(ASynchronization *pSynchObject)
{
  pDelete(mp_SynchObject);
  if (pSynchObject)
    mp_SynchObject = pSynchObject;
}

void AObjectBase::lock()
{
  if (mp_SynchObject)
    mp_SynchObject->lock();
  else
    ATHROW(this, AException::MemberNotInitialized);
}

bool AObjectBase::trylock()
{
  if (mp_SynchObject)
    return mp_SynchObject->trylock();
  else
    ATHROW(this, AException::MemberNotInitialized);
}

void AObjectBase::unlock()
{
  if (mp_SynchObject)
    mp_SynchObject->unlock();
  else
    ATHROW(this, AException::MemberNotInitialized);
}
