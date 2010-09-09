/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ABase.hpp"
#include "ALock.hpp"
#include "AString.hpp"
#include "ATime.hpp"
#include "templateMapOfPtrs.hpp"

#ifdef __WINDOWS__
#include "Dbghelp.h"
#endif

ABase::ABase() :
  pNext(NULL),
  pPrev(NULL)
{
}

ABase::~ABase()
{
}

void ABase::setNext(ABase *p)
{
  pNext = p;
}

void ABase::setPrev(ABase *p)
{
  pPrev = p;
}

const ABase *ABase::getNext() const
{
  return pNext;
}

const ABase *ABase::getPrev() const
{
  return pPrev;
}

ABase *ABase::useNext()
{
  return pNext;
}

ABase *ABase::usePrev()
{
  return pPrev;
}

ABase *ABase::unlink()
{
  if (pPrev)
    pPrev->pNext = pNext;

  if (pNext)
  {
    pNext->pPrev = pPrev;
    pNext = NULL;
  }

  pPrev = NULL;

  return this;
}
