/*
Written by Alex Chachanashvili

$Id$
*/
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

  Usage:
    AStringHashMap mymap(new ASync_Mutex("MyHashMap"));  // synchronized by a mutex

    or

    AStringHashMap mymap(NULL, 29);  //unsynchronized with larger hash table

  @param pSynchObject used for synchronizing access, is OWNED and DELETED by this class, if NULL it is unsynchronized
  @param hashSize should be a prime number (1,2,3,5,7,11,13,17,19,23,29,etc)
  */
  AStringHashMap(ASynchronization *pSynchObject = NULL, u1 hashSize = 13);
  
  /*! dtor */
  virtual ~AStringHashMap();

  /*!
  Set name/value pair
  @param name key
  @param value data
  */
  void set(const AString& name, const AString& value);

  /*!
  Set name/value pair
  Uses the rope size to pre-allocate the value

  @param name key
  @param value data
  */
  void set(const AString&, const ARope& value);


  /*!
  Set name/value pair
  Size of AEmittable is unknown so will allocate on the fly

  @param name key
  @param value data
  */
  void   set(const AString& name, const AEmittable& value);
  
  /*!
  Get the value at the name key
  If found value will be appended to target

  @param name key
  @param target to append value to
  @return true if name key was found
  */
  bool get(const AString& name, AOutputBuffer& target) const;
  
  /*!
  Checks if name/value pair exists

  @param name key
  @return true if name key found
  */
  bool exists(const AString& name) const;
  
  
  /*!
  Remove name/value pair

  @param name key
  @return true if name key found and removed
  */
  bool remove(const AString&);
  
  /*!
  Count number of name/value pairs
  
  @return count
  */
  size_t getSize() const;
  
  /*!
  Clear all name/value pairs
  */
  void clear();

  /*!
  Allows direct access to the stored AString
  Can throw AException if not found

  @param name key
  @return reference to the value
  @throw exception if not found
  */
  AString& use(const AString& name);

  /*!
  Allows direct access to the stored AString
  Can throw AException if not found

  @param name key
  @return constant reference to the value
  @throw exception if not found
  */
  const AString& get(const AString& name) const;

  /*!
  ASerializable
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

  /*!
  AEmittable
  
  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable

  @param thisRoot XML element that represents this class to append to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

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
