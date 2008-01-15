#ifndef INCLUDED__ABase_hpp__
#define INCLUDED__ABase_hpp__

#include "apiABase.hpp"

/*!
Simple common base class

Allows use of dynamic_cast<> since void* is not allowed
Has a pointer to next and previous for easy collection building
*/
class ABASE_API ABase
{
public:
  /*!
  ctor

  pNext and pPrev used if building a collection, NULL otherwise
  */
  ABase(ABase *pNext = NULL, ABase *pPrev = NULL);

  //! dtor
  virtual ~ABase();

  /*!
  Get next
  */
  ABase *getNext() const;

  /*!
  Get prev
  */
  ABase *getPrev() const;

  /*!
  Set next
  */
  void setNext(ABase *);

  /*!
  Set prev
  */
  void setPrev(ABase *);

protected:
  ABase *mp_Next;
  ABase *mp_Prev;
};

#endif // INCLUDED__ABase_hpp__

