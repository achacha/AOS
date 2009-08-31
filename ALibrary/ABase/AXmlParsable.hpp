/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AXmlParsable_HPP_
#define INCLUDED__AXmlParsable_HPP_

#include "apiABase.hpp"

class AXmlElement;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void fromXml(const AXmlElement&);
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API AXmlParsable
{
public:
  //! virtual dtor
  virtual ~AXmlParsable();

  /*!
  Ability to parse self from XML element

  @param source to parse XML from
  */
  virtual void fromXml(const AXmlElement& source) = 0;
};

#endif //INCLUDED__AXmlParsable_HPP_
