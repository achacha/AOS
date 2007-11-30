#ifndef INCLUDED__AOSSessionData_HPP__
#define INCLUDED__AOSSessionData_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AStringHashMap.hpp"
#include "ATimer.hpp"
#include "AXmlEmittable.hpp"
#include "ASerializable.hpp"

class AOS_BASE_API AOSSessionData : public ADebugDumpable, public AXmlEmittable, ASerializable
{
public:
  static const AString SESSIONID;       //a_Session ID stored in data with this key

public:
  /*!
  ctor
  @param sessionId of this data and is added to the data as 'id' parameter
  @param number of buckets in the data hash map (prime number better)
  */
  AOSSessionData(const AString& sessionId, size_t defaultDataHashSize = 5);
  
  /*!
  ctor calls fromAFile
  */
  AOSSessionData(AFile&);

  /*!
  Gets the session ID stored in data
  */
  const AString& getSessionId() const;

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  Data is all AString->AString map
  Persiatable if needed via ASerialization interface

  @return Data
  */
  AStringHashMap& useData();
  
  /*!
  Overall age timer of the session
  */
  const ATimer& getAgeTimer() const;
  
  /*!
  Last used timer
  Gets reset when object is de-serialized via fromAFile
  */
  const ATimer& getLastUsedTimer() const;

  /*!
  Timer reset for last time used
  */
  void restartLastUsedTimer();

  /*!
  ASerializable
  NOTE: Timers are not persisted and are reset upon deserialization
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

private:
  AOSSessionData() {}

  AStringHashMap m_Data;
  ATimer m_AgeTimer;
  ATimer m_LastUsedTimer;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSSessionData_HPP__
