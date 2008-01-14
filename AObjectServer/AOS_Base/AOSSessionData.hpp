#ifndef INCLUDED__AOSSessionData_HPP__
#define INCLUDED__AOSSessionData_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlDocument.hpp"
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

  @param target appends session id if found
  @return true if found
  */
  bool AOSSessionData::getSessionId(AOutputBuffer& target) const;

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  Data associated with the session
  Persiatable if needed via ASerialization interface

  @return Data
  */
  AXmlElement& useData();
  const AXmlElement& getData() const;
  
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
  Reset the object timers and clear the data
  */
  void clear();

  /*!
  ASerializable
  NOTE: Timers are not persisted and are reset upon deserialization
  */
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AXmlDocument m_Data;
  ATimer m_AgeTimer;
  ATimer m_LastUsedTimer;
};

#endif //INCLUDED__AOSSessionData_HPP__
