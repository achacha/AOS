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
Internally implemented as a double linked list

Container does NOT delete anything implicitly when destroyed
MUST call clear(true) to delete contents and clear container

Optimzed for push/pop operations

This is a very basic queue that is thread safe for pop/push/clear operations
You can use head/tail but you MUST use sync when traversing it to avoid race conditions
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
  Get ABase * at given index from front

  @param index of the element from front (0 based)
  @return access to ABase * (NULL if list empty or index out of bounds)
  */
  const ABase *getAt(size_t index) const;
  template<class T> T* getAt(size_t index) { return dynamic_cast<T *>(getAt(index)); }
  
  /*!
  Use ABase * at given index from front

  @param index of the element from front (0 based)
  @return access to ABase * (NULL if list empty or index out of bounds)
  */
  ABase *useAt(size_t index);
  template<class T> T* useAt(size_t index) { return dynamic_cast<T *>(useAt(index)); }

  /*!
  Pop object from the front

  @param index of the element from front (0 based)
  @return removed ABase * (NULL if list empty or index out of bounds)
  */
  ABase *popFront(size_t index = 0);
  template<class T> T* popFront(size_t index = 0)  { return dynamic_cast<T *>(popFront(index)); }

  /*!
  Pop object from the back

  @param index of the element from back (0 based)
  @return removed ABase * (NULL if list empty or index out of bounds)
  */
  ABase *popBack(size_t index = 0);
  template<class T> T* popBack(size_t index = 0)  { return dynamic_cast<T *>(popBack(index)); }

  /*!
  Push object to front

  @param p ABase * to add to the queue, object is OWNED and DELETED by the queue
  @return p for convenience
  */
  ABase *pushFront(ABase *p);

  /*!
  Push object to back

  @param p ABase * to add to the queue, object is OWNED and DELETED by the queue
  @return p for convenience
  */
  ABase *pushBack(ABase *p);

  /*!
  Find ABase * pointer in the queue starting with front

  @param p ABase * to find
  @return index from front to p or AConstant::npos if not found
  */
  size_t findFromFront(ABase *p);

  /*!
  Find ABase * pointer in the queue starting with back

  @param p ABase * to find
  @return index from back to p or AConstant::npos if not found
  */
  size_t findFromBack(ABase *p);

  /*!
  Checks if queue is empty

  @return true if empty
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
  NOT thread safe
  NOTE: Use sync to guarantee thread safety

  @return head pointer
  */
  ABase *useHead();
  template<class T> T* useHead()  { return dynamic_cast<T *>(useHead()); }

  /*!
  Access the head pointer
  NOT thread safe
  NOTE: Use sync to guarantee thread safety

  @return tail pointer
  */
  ABase *useTail();
  template<class T> T* useTail() { return dynamic_cast<T *>(useTail()); }

  /*!
  Access the head pointer
  NOT thread safe
  NOTE: Use sync to guarantee thread safety

  @return constant head pointer
  */
  const ABase *getHead() const;
  template<class T> const T* getHead() const { return dynamic_cast<T *>(getHead()); }

  /*!
  Access the head pointer
  NOT thread safe
  NOTE: Use sync to guarantee thread safety
  
  @return constant tail pointer
  */
  const ABase *getTail() const;
  template<class T> const T* getTail() const { return dynamic_cast<T *>(getTail()); }

  /*!
  Removes pointer from container
  Does not verify that it is actually part of this queue at runtime (does in debug)
    NOTE: if not part of this queue then size will be inconsistent and size may go negative
  Unlinks the ABase* and then makes sure it is not head or tail of this container

  @param p ABase*
  @return p for convenience
  */
  ABase *remove(ABase *p);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! Internal accessor
  ABase *_getAt(size_t index) const;
  
  //! Internal unsynchronized remove
  ABase *_remove(ABase *p);

private:
  ASynchronization *mp_Sync;

  ABase *mp_Head;
  ABase *mp_Tail;
  size_t m_Size;
};

#endif // INCLUDED__ABasePtrQueue_hpp__
