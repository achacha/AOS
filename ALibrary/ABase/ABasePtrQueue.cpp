/*
Written by Alex Chachanashvili

$Id$
*/
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
  try
  {
    delete mp_Sync;
  }
  catch(...) {}
}

const ABase *ABasePtrQueue::getAt(size_t index) const
{
  ALock lock(mp_Sync);
  return _getAt(index);
}

ABase *ABasePtrQueue::useAt(size_t index)
{
  ALock lock(mp_Sync);
  return _getAt(index);
}

ABase *ABasePtrQueue::_getAt(size_t index) const
{
  size_t pos = 0;
  ABase *p = NULL;
  if (index > (m_Size >> 1))
  {
    // Go backwards
    p = mp_Tail;
    index = m_Size - index - 1;
    while(p && pos < index)
    {
      p = p->pPrev;
      ++pos;
    }
  }
  else
  {
    // Go forwards
    p = mp_Head;
    while(p && pos < index)
    {
      p = p->pNext;
      ++pos;
    }
  }
  return p;
}

ABase *ABasePtrQueue::popFront(size_t index)
{
  ALock lock(mp_Sync);

  //a_Out of bounds
  if (index >= m_Size)
    return NULL;

  // Pop from offset
  ABase *p = _getAt(index);
  if (p)
    return _remove(p);
  else
    return p;
}

ABase *ABasePtrQueue::popBack(size_t index)
{
  ALock lock(mp_Sync);

  //a_Out of bounds
  if (index >= m_Size)
    return NULL;

  // Pop from offset
  ABase *p = _getAt(m_Size - index - 1);
  if (p)
    return _remove(p);
  else
    return p;
}

ABase *ABasePtrQueue::_remove(ABase *p)
{
  AASSERT(this, p);
  if (p->pNext)
  {
    if (p->pPrev)
    {
      // Has prev and next
      p->pNext->pPrev = p->pPrev;
      p->pPrev->pNext = p->pNext;
      p->pNext = NULL;
      p->pPrev = NULL;
    }
    else
    {
      // Has next and no prev (first in list)
      AASSERT(this, p == mp_Head);
      mp_Head = p->pNext;
      mp_Head->pPrev = NULL;
      p->pNext = NULL;
    }
  }
  else
  {
    // This is the last one
    if (p->pPrev)
    {
      // Has prev and no next
      AASSERT(this, p == mp_Tail);
      mp_Tail = p->pPrev;
      mp_Tail->pNext = NULL;
      p->pPrev = NULL;
    }
    else
    {
      // no next and no prev
      AASSERT(this, mp_Head == mp_Tail);
      mp_Head = NULL;
      mp_Tail = NULL;
    }
  }

  AASSERT(this, m_Size > 0);
  --m_Size;
  
  return p;
}

ABase *ABasePtrQueue::remove(ABase *p)
{
  ALock lock(mp_Sync);

  AASSERT(this, findFromFront(p) >= 0);
  
  return _remove(p);
}

ABase *ABasePtrQueue::pushFront(ABase *p)
{
  AASSERT(this, m_Size<DEBUG_MAXSIZE_ABasePtrQueue);  //a_Debug only limit
  
  ALock lock(mp_Sync);
  if (mp_Head)
  {
    //a_Queue not empty, append item
    p->pNext = mp_Head;
    mp_Head->pPrev = p;
    mp_Head = p;
    mp_Head->pPrev = NULL;

    AASSERT(this, m_Size>0);
    ++m_Size;
  }
  else
  {
    //a_Queue empty
    AASSERT(this, !mp_Tail);
    AASSERT(this, 0 == m_Size);
    ++m_Size;

    p->pNext = NULL;
    p->pPrev = NULL;
    mp_Tail = p;
    mp_Head = p;
  }
  return p;
}

ABase *ABasePtrQueue::pushBack(ABase *p)
{
  AASSERT(this, m_Size<DEBUG_MAXSIZE_ABasePtrQueue);  //a_Debug only limit
  
  ALock lock(mp_Sync);
  if (mp_Tail)
  {
    //a_Queue not empty, append item
    p->pPrev = mp_Tail;
    mp_Tail->pNext = p;
    mp_Tail = p;
    mp_Tail->pNext = NULL;

    AASSERT(this, m_Size>0);
    ++m_Size;
  }
  else
  {
    //a_Queue empty
    AASSERT(this, !mp_Head);
    AASSERT(this, 0 == m_Size);
    ++m_Size;

    p->pNext = NULL;
    p->pPrev = NULL;
    mp_Tail = p;
    mp_Head = p;
  }
  return p;
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

size_t ABasePtrQueue::findFromFront(ABase *p)
{
  AASSERT(this, p);

  ALock lock(mp_Sync);
  size_t pos = 0;
  ABase *pX = mp_Head;
  while (pX)
  {
    if (pX == p)
      return pos;

    pX = pX->pNext;
    ++pos;
  }
  return AConstant::npos;
}

size_t ABasePtrQueue::findFromBack(ABase *p)
{
  AASSERT(this, p);

  ALock lock(mp_Sync);
  size_t pos = 0;
  ABase *pX = mp_Tail;
  while (pX)
  {
    if (pX == p)
      return pos;

    pX = pX->pPrev;
    ++pos;
  }
  return AConstant::npos;
}
