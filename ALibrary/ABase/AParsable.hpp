/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AParsable_HPP_
#define INCLUDED__AParsable_HPP_

#include "apiABase.hpp"
#include "AEmittable.hpp"

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void parse(const AEmittable&);
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API AParsable
{
public:
  virtual ~AParsable();

  /*!
  Ability to parse self from AEmittable
  Intended as a compliment to AEmittable
  */
  virtual void parse(const AEmittable&) = 0;
};

#endif //INCLUDED__AParsable_HPP_
