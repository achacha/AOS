#ifndef INCLUDED__AXmlParsable_HPP_
#define INCLUDED__AXmlParsable_HPP_

#include "apiABase.hpp"

class AXmlNode;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void parse(const AXmlNode&);
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API AXmlParsable
{
public:
  virtual ~AXmlParsable();

  /*!
  Ability to parse self from XML element
  */
  virtual void parse(const AXmlNode&) = 0;
};

#endif //INCLUDED__AXmlParsable_HPP_
