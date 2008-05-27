/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "ABase.hpp"

ABase::ABase() :
  pPrev(NULL),
  pNext(NULL)
{
}

ABase::~ABase()
{
}

ABase *ABase::useNext()
{
  return pNext;
}

ABase *ABase::usePrev()
{
  return pPrev;
}

const ABase *ABase::getNext() const
{
  return pNext;
}

const ABase *ABase::getPrev() const
{
  return pPrev;
}
