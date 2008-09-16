/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlEmittable_HPP_
#define INCLUDED__AXmlEmittable_HPP_

#include "apiABase.hpp"

class AXmlElement;

/*!
  Interface for a class to emit itself onto an XML model
  The element passed in is always the root for this object
  The parent always names the element of the object then calls its emitXML


-------------------START: HPP cup/past----------------------
public:
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;
-------------------STOP: HPP cup/past-----------------------

-------------------START: CPP cup/past----------------------
AXmlElement& MyClass::emitXml(AXmlElement& thisRoot) const
{
  //thisRoot.useAttributes().insert("name", "value");
  //thisRoot.addElement("name").addData("value");
  
  return thisRoot;
}
-------------------STOP: CPP cup/past-----------------------
*/
class ABASE_API AXmlEmittable
{
public:
  //! virtual dtor
  virtual ~AXmlEmittable();

  /*!
  Ability to emit self as XML tree

  @param thisRoot is the base element of the object and sub-elements are actual members of this object
  @return thisRoot is returned for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const = 0;
};

#endif //INCLUDED__AXmlEmittable_HPP_
