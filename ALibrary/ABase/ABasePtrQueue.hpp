/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ABasePtrQueue_hpp__
#define INCLUDED__ABasePtrQueue_hpp__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

class ASynchronization;

/*!
Queue of ABase* types

Optimzed for push/pop operations

This is a very basic queue that is thread safe for pop/push/clear operations
You can manually access head/tail but you MUST use sync when traversing it to avoid race conditions
*/
class ABASE_API ABasePtrQueue : public ADebugDumpable
{
public:
  /*!
  ctor

  @param pSync if non-NULL will be used to synchronize queue operations (will be OWNED and DELETED when done)

  Usage: ABasePtrQueue queue(new ACriticalSection());
  */
  ABasePtrQueue(ASynchronization *pSync = NULL);

  //! dtor
  virtual ~ABasePtrQueue();

  /*!
  Pop object from the front

  @return ABase * or NULL if empty
  */
  ABase *pop();

  /*!
  Push object to back

  @param p ABase * to add to the queue, object is OWNED and DELETED by the queue
  */
  void push(ABase *p);

  /*!
  Checks if queue is empty
  */
  bool isEmpty() const;

  /*!
  Clear the queue

  @param deleteContent if true will call delete on all contained objects, if not the queue is just cleared (which may leak memory if objects are owned)
  */
  void clear(bool deleteContent = true);

  /*!
  Get the current size of the queue

  @return size
  */
  size_t size() const;

  /*!
  Access synchronization pointer

  @return NULL if unsynchronized

  Usage:
    \code
    {
      ALock lock(queue.getSync());
      // do so operations
    }
    \endcode
  */
  ASynchronization *useSync();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety

  @return head pointer
  */
  ABase *useHead();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety

  @return tail pointer
  */
  ABase *useTail();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety

  @return constant head pointer
  */
  const ABase *getHead() const;

  /*!
  Access the head pointer
  Use sync to guarantee thread safety
  
  @return constant tail pointer
  */
  const ABase *getTail() const;

  /*!
  Removes pointer from container
  Does not verify that it is actually part of this queue, if not part of this queue then size
    will be inconsistent and size may go negative
  Unlinks the ABase* and then makes sure it is not head or tail of this container

  @param p ABase*
  */
  void remove(ABase *p);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  ASynchronization *mp_Sync;

  ABase *mp_Head;
  ABase *mp_Tail;
  size_t m_Size;
};

#endif // INCLUDED__ABasePtrQueue_hpp__
