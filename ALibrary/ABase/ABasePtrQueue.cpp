#include "pchABase.hpp"
#include "ABasePtrQueue.hpp"
#include "ABasePtrHolder.hpp"
#include "ASynchronization.hpp"

void ABasePtrQueue::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Sync=" << AString::fromPointer(mp_Sync) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Head=" << AString::fromPointer(mp_Head) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Tail=" << AString::fromPointer(mp_Tail) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ABasePtrQueue::ABasePtrQueue(ASynchronization *pSync) :
  mp_Sync(pSync),
  mp_Head(NULL),
  mp_Tail(NULL)
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
  }
  return ret;
}

void ABasePtrQueue::push(ABase *p)
{
  ALock lock(mp_Sync);
  if (mp_Tail)
  {
    //a_Queue not empty, append item
    mp_Tail->pNext = p;
    mp_Tail = p;
    mp_Tail->pNext = NULL;
  }
  else
  {
    p->pNext = NULL;
    mp_Tail = p;
    mp_Head = p;
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
    mp_Head = NULL;
  }
  mp_Tail = NULL;
}

bool ABasePtrQueue::isEmpty() const
{
  ALock lock(mp_Sync);
  return (NULL == mp_Head);
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
