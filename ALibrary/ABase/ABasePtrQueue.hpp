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

  @param ABase * to add to the queue
  */
  void push(ABase *);

  /*!
  Checks if queue is empty
  */
  bool isEmpty() const;

  /*!
  Clear the queue
  */
  void clear();

  /*!
  Access synchronization pointer

  @return NULL if unsynchronized

  Usage:
    {
      ALock lock(queue.getSync());
      // do so operations
    }
  */
  ASynchronization *useSync();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety
  */
  ABase *useHead();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety
  */
  ABase *useTail();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  ASynchronization *mp_Sync;

  ABase *mp_Head;
  ABase *mp_Tail;
};

#endif // INCLUDED__ABasePtrQueue_hpp__
