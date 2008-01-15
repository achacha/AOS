#ifndef INCLUDED__ABasePtrQueue_hpp__
#define INCLUDED__ABasePtrQueue_hpp__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

class ASynchronization;

/*!
Queue of ABase* types

Optimzed for push/pop operations
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
  Access synchronization pointer

  @return NULL if unsynchronized

  Usage:
    {
      ALock lock(queue.getSync());
      // do so operations
    }
  */
  ASynchronization *getSync();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety
  */
  ABase *getHead();

  /*!
  Access the head pointer
  Use sync to guarantee thread safety
  */
  ABase *getTail();

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
