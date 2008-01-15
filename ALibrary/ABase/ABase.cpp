#include "pchABase.hpp"
#include "ABase.hpp"

ABase::ABase(
  ABase *pNext,  // = NULL
  ABase *pPrev   // = NULL
) :
  mp_Next(pNext),
  mp_Prev(pPrev)
{
}

ABase::~ABase()
{
}

ABase *ABase::getNext() const
{
  return mp_Next;
}

ABase *ABase::getPrev() const
{
  return mp_Prev;
}

void ABase::setNext(ABase *p)
{
  mp_Next = p;
}

void ABase::setPrev(ABase *p)
{
  mp_Prev = p;
}
