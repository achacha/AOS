#ifndef INCLUDED__AOSUser_HPP__
#define INCLUDED__AOSUser_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "ASerializable.hpp"
#include "AStringHashMap.hpp"

/*!
This class is a pure wrapper to AXmlElement and uses it to look up and change data
It is not intended to have any local members other than transients
This is a view into the model that contains the user xml data
*/
class AOS_BASE_API AOSUser
{
public:
  AOSUser(AXmlElement&);

  /*!
  Data for this user
  */
  AXmlElement& useElement();
  const AXmlElement& getElement() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Username
  AXmlElement& m_Element;
};

#endif // INCLUDED__AOSUser_HPP__
