#include "pchABase.hpp"
#include "ABasePtrQueue.hpp"
#include "ASynchronization.hpp"

void ABasePtrQueue::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Sync=" << AString::fromPointer(mp_Sync) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Head=" << AString::fromPointer(mp_Head)
    << "  mp_Tail=" << AString::fromPointer(mp_Tail) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Size=" << m_Size << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ABasePtrQueue::ABasePtrQueue(ASynchronization *pSync) :
  mp_Sync(pSync),
  mp_Head(NULL),
  mp_Tail(NULL),
  m_Size(0)
{
}

ABasePtrQueue::~ABasePtrQueue()
{
}

ABase *ABasePtrQueue::pop()
{
  ABase *ret = NULL;
  ALock lock(mp_Sync);
  if (mp_Head)
  {
    //a_Queue not empty
    ret = mp_Head;
    if (!(mp_Head = mp_Head->pNext))
      mp_Tail = NULL;    //a_Queue was emptied
  
    AASSERT(this, m_Size>0);
    --m_Size;
  }
  return ret;
}

void ABasePtrQueue::push(ABase *p)
{
  AASSERT(this, m_Size<DEBUG_MAXSIZE_ABasePtrQueue);  //a_Debug only limit
  
  ALock lock(mp_Sync);
  if (mp_Tail)
  {
    //a_Queue not empty, append item
    mp_Tail->pNext = p;
    mp_Tail = p;
    mp_Tail->pNext = NULL;

    AASSERT(this, m_Size>0);
    ++m_Size;
  }
  else
  {
    p->pNext = NULL;
    mp_Tail = p;
    mp_Head = p;

    AASSERT(this, 0 == m_Size);
    ++m_Size;
  }
}

void ABasePtrQueue::clear(bool deleteContent)
{
  ALock lock(mp_Sync);
  if (mp_Head && deleteContent)
  {
    ABase *pNext;
    ABase *pKill = mp_Head;
    while (pKill)
    {
      pNext = pKill->pNext;
      delete pKill;
      pKill = pNext;
    } 
  }
  mp_Head = NULL;
  mp_Tail = NULL;
  m_Size = 0;
}

bool ABasePtrQueue::isEmpty() const
{
  ALock lock(mp_Sync);
  return (NULL == mp_Head);
}

size_t ABasePtrQueue::size() const
{
  ALock lock(mp_Sync);
  return m_Size;
}

ASynchronization *ABasePtrQueue::useSync()
{
  return mp_Sync;
}

ABase *ABasePtrQueue::useHead()
{
  return mp_Head;
}

ABase *ABasePtrQueue::useTail()
{
  return mp_Tail;
}

const ABase *ABasePtrQueue::getHead() const
{
  return mp_Head;
}

const ABase *ABasePtrQueue::getTail() const
{
  return mp_Tail;
}
