/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ABase_hpp__
#define INCLUDED__ABase_hpp__

#include "apiABase.hpp"

class ASynchronization;
class ABasePtrQueue;
class AOutputBuffer;

/*!
Simple common base class

Allows use of dynamic_cast<> since void* is not allowed
Has a pointer to next and previous for easy collection building
*/
class ABASE_API ABase
{
  /*!
  Friend class allowed access to collection pointers pNext and pPrev
  */
  friend class ABasePtrQueue;

public:
  /*!
  ctor
  */
  ABase();

  //! dtor
  virtual ~ABase();

  /*!
  Get the next item in the chain
  @return NULL if none
  */
  ABase *useNext();

  /*!
  Get the previous item in the chain
  @return NULL if none
  */
  ABase *usePrev();

  /*!
  Get the next item in the chain
  @return NULL if none
  */
  const ABase *getNext() const;

  /*!
  Get the previous item in the chain
  @return NULL if none
  */
  const ABase *getPrev() const;

/*!
If DEBUG_TRACK_ABASE_MEMORY is defined all allocation/deallocation via new/new[]/delete/delete[] of ABase objects is traced to std::cout
*/
#ifdef DEBUG_TRACK_ABASE_MEMORY
  void *operator new(size_t size);
  void operator delete(void *p);
  void *operator new[](size_t size);
  void operator delete[](void *p);
  void traceAllocations(AOutputBuffer&, bool includeHexDump);
#endif

protected:
  /*!
  Next pointer in some collection
  */
  ABase *pNext;
  
  /*!
  Previous pointer in some collection
  */
  ABase *pPrev;
};

#endif // INCLUDED__ABase_hpp__

