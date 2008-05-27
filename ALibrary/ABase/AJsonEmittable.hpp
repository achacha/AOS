/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AJsonEmittable_HPP_
#define INCLUDED__AJsonEmittable_HPP_

#include "apiABase.hpp"

class AOutputBuffer;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void emitJson(AOutputBuffer&) const;
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API AJsonEmittable
{
public:
  virtual ~AJsonEmittable();

  /*!
  Ability to emit self as JavaScript Object Notation (JSON)
  */
  virtual void emitJson(AOutputBuffer&, int indent = -1) const = 0;
};

#endif //INCLUDED__AJsonEmittable_HPP_
