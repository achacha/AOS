#ifndef INCLUDED__AOSUser_HPP__
#define INCLUDED__AOSUser_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "AXmlParsable.hpp"
#include "ASerializable.hpp"
#include "AStringHashMap.hpp"

/*!
Generic user object

Everything is stored in data
*/
class AOS_BASE_API AOSUser : public AXmlEmittable, public AXmlParsable, public ASerializable
{
public:
  static const AString USERNAME;
  static const AString PASSWORD;

public:
  AOSUser();
  virtual ~AOSUser();

  /*!
  Data for this user
  */
  AStringHashMap& useData();
  const AStringHashMap& getData() const;

  /*!
  AXmlParsable
  */
  virtual void fromXml(const AXmlElement&);

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  ASerializable
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

private:
  //! Username
  AString m_Username;

  //! Password (SHA1 hash)
  AString m_Password;

  //! Data
  AStringHashMap m_Data;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSUser_HPP__
