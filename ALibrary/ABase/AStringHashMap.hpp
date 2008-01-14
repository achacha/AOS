#ifndef INCLUDED__AStringHashMap_HPP__
#define INCLUDED__AStringHashMap_HPP__

#include "apiABase.hpp"
#include "ASerializable.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"
#include "ASynchronization.hpp"

/*!
Generic container
( AString name -> AString value )

name lookup case-sensitive
name must be unique
name storage bin based on hash value

AEmittable::emit() outputs "name=value&name=value=..."
*/
class ABASE_API AStringHashMap : public ASerializable, public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Construct based on hash intended for a large collection of data
  hash size should be a prime number (1,2,3,5,7,11,13,17,19,23,29,etc)

  pSynchObject is OWNED and DELETED by this class, if NULL it is unsynchroinized

  Usage:
    AStringHashMap mymap(new ASync_Mutex("MyHashMap"));  // synchronized by a mutex

    or

    AStringHashMap mymap(NULL, 29);  //unsynchronized with larger hash table
  */
  AStringHashMap(ASynchronization *pSynchObject = NULL, u1 hashSize = 13);
  virtual ~AStringHashMap();

  /*!
  Set name/value pair
    If AEmittable, it will call emit() and insert AString that results
  */
  void   set(const AString&, const AString&);
  void   set(const AString&, const ARope&);
  void   set(const AString&, const AEmittable&);
  bool   get(const AString&, AOutputBuffer&) const;     //a_Does not clear target
  bool   exists(const AString&) const;
  bool   remove(const AString&);
  size_t getSize() const;
  void   clear();

  /*!
  Allows direct access to the stored AString
  Can throw AException if not found
  */
  AString& use(const AString&);
  const AString& get(const AString&) const;

  /*!
  ASerializable
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  typedef std::vector<MAP_AString_AString *> HASH_MAP;
  HASH_MAP m_Container;

  void _parse(const AString&);

  ASynchronization *mp_SynchObject;
};

#endif // INCLUDED__AStringPathHashMap_HPP__
