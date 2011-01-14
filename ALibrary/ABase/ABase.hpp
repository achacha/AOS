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
  Set the next item in the chain
  
  @param p ABase * or NULL if none
  */
  void setNext(ABase *p);

  /*!
  Set the previous item in the chain
  
  @param p ABase * or NULL if none
  */
  void setPrev(ABase *p);

  /*!
  Get the next item in the chain
  
  @return NULL if none
  */
  const ABase *getNext() const;
  template<class T> const T *getNext() const { return dynamic_cast<T *>(getNext()); }

  /*!
  Get the previous item in the chain
  
  @return NULL if none
  */
  const ABase *getPrev() const;
  template<class T> const T *getPrev() const { return dynamic_cast<T *>(getPrev()); }

  /*!
  Use the next item in the chain
  
  @return NULL if none
  */
  ABase *useNext();
  template<class T> T *useNext() { return dynamic_cast<T *>(useNext()); }

  /*!
  Use the previous item in the chain
  
  @return NULL if none
  */
  ABase *usePrev();
  template<class T> T *usePrev() { return dynamic_cast<T *>(usePrev()); }

  /*!
  Unlink current item from a list
  Assigns next of the previous to the next of this effectively removing it from a list

  Effectively: 
    this->getPrev().setNext(this->getNext());
    this->getNext().setPrev(this->getPrev());
  If this->getPrev() is NULL, then previous' next is set to NULL
  If this->getNext() is NULL, then next's previous is set to NULL

  This item is not unlinked from the linked list it was part of and previous and next are set to NULL

  @return this for convenience
  */
  ABase *unlink();
  template<class T> T *unlink() { return dynamic_cast<T *>(unlink()); }

/*!
If DEBUG_TRACK_ABASE_MEMORY is defined all allocation/deallocation via new/new[]/delete/delete[] of ABase objects is traced to std::cout
*/
#ifdef DEBUG_TRACK_ABASE_MEMORY
  void *operator new(size_t size);
  void operator delete(void *p);
  static void traceAllocations(AOutputBuffer&, bool includeHexDump);
  static void clearAllocations();
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

