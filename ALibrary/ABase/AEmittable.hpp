/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AEmittable_HPP_
#define INCLUDED__AEmittable_HPP_

#include "apiABase.hpp"
#include "ABase.hpp"

class AOutputBuffer;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void emit(AOutputBuffer&) const;
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API AEmittable : public ABase
{
public:
  //! virtual dtor
  virtual ~AEmittable();

  /*!
  Ability to emit self, always appends to the AOutputBuffer object

  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const = 0;
};

#endif //INCLUDED__AEmittable_HPP_
