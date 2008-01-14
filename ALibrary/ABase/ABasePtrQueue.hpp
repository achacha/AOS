#ifndef INCLUDED__ABasePtrQueue_hpp__
#define INCLUDED__ABasePtrQueue_hpp__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

/*!
Queue of ABase* types

Optimzed for push/pop operations
*/
class ABASE_API ABasePtrQueue : public ADebugDumpable
{
public:
  //! ctor
  ABasePtrQueue();

  //! dtor
  virtual ~ABasePtrQueue();

  /*!
  Push object to the front
  */
  void pushFront(ABase *);

  /*!
  Pop object from the front
  */
  ABase *popFront();

  /*!
  Push object to back
  */
  void pushBack(ABase *);

  /*!
  Pop object from back
  */
  ABase *popBack();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
};

#endif // INCLUDED__ABasePtrQueue_hpp__
