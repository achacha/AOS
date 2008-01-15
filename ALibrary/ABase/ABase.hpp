#ifndef INCLUDED__ABase_hpp__
#define INCLUDED__ABase_hpp__

#include "apiABase.hpp"

class ABasePtrQueue;

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

