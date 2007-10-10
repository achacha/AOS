#ifndef INCLUDED__AXmlEmittable_HPP_
#define INCLUDED__AXmlEmittable_HPP_

#include "apiABase.hpp"

class AXmlElement;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void emitXml(AXmlElement&) const;
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API AXmlEmittable
{
public:
  virtual ~AXmlEmittable();

  /*!
  Ability to emit self as XML tree
  */
  virtual void emitXml(AXmlElement&) const = 0;
};

#endif //INCLUDED__AXmlEmittable_HPP_
