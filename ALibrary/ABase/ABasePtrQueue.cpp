#include "pchABase.hpp"
#include "ABasePtrQueue.hpp"
#include "ABasePtrHolder.hpp"
#include "ASynchronization.hpp"

void ABasePtrQueue::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Sync=" << AString::fromPointer(mp_Sync) << std::endl;
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
  ALock lock(mp_Sync);
  if (mp_Head)
  {
    //a_Queue not empty
    ABase *ret = mp_Head;
    mp_Head = mp_Head->getNext();
    if (!mp_Head)
      mp_Tail = NULL;    //a_Queue was emptied
  }
  else
    return NULL;
}

void ABasePtrQueue::push(ABase *p)
{
  ALock lock(mp_Sync);
  if (mp_Tail)
  {
    //a_Queue not empty, append item
    mp_Tail->setNext(p);
    mp_Tail = p;
    mp_Tail->setNext(NULL);
  }
  else
  {
    mp_Tail = p;
    mp_Head = p;
  }
}

ASynchronization *ABasePtrQueue::getSync()
{
  return mp_Sync;
}

ABase *ABasePtrQueue::getHead()
{
  return mp_Head;
}

ABase *ABasePtrQueue::getTail()
{
  return mp_Tail;
}

