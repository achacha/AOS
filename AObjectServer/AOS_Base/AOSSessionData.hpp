/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSSessionData_HPP__
#define INCLUDED__AOSSessionData_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlDocument.hpp"
#include "ATimer.hpp"
#include "AXmlEmittable.hpp"
#include "ASerializable.hpp"

class AOSContext;
class AOSSessionMapHolder;

class AOS_BASE_API AOSSessionData : public ADebugDumpable, public AXmlEmittable, public ASerializable
{
public:
  
  // Session ID stored in data with this key
  static const AString SESSIONID;       
  
  // Session path where locale override can be stored, used during directory lookup
  static const AString LOCALE;          
  
  // Name of the root element for session data
  static const AString ROOT;

public:
  /*!
  ctor

  @param sessionId of this data and is added to the data as 'id' parameter
  @param holder of this data
  */
  AOSSessionData(const AString& sessionId, AOSSessionMapHolder& holder);
  
  /*!
  ctor calls fromAFile

  @param file to read session from
  @param holder of this data
  */
  AOSSessionData(AFile& file, AOSSessionMapHolder& holder);

  /*!
  dtor
  */
  virtual ~AOSSessionData();

  /*!
  Gets the session ID stored in data

  @param target appends session id if found
  @return true if found
  */
  bool getSessionId(AOutputBuffer& target) const;

  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Data associated with the session
  Persiatable if needed via ASerialization interface

  @return Reference to the session data
  */
  AXmlElement& useData();

  /*!
  Data associated with the session
  Persiatable if needed via ASerialization interface

  @return Constant reference to the session data
  */
  const AXmlElement& getData() const;
  
  /*!
  Get a synchronization object associated with THIS data
  
  @return Sync object of this instance
  */
  ASynchronization *useSyncObject();

  /*!
  Initialize the data for each request
  Calling code responsible for syncronizing this call

  @param context RequestContext
  */
  void init(AOSContext& context);
  
  /*!
  Finalize the usage for the given request
  Calling code responsible for syncronizing this call

  @param context RequestContext
  */
  void finalize(AOSContext& context);

  /*!
  Overall age timer of the session

  @return Constant reference to the ATimer object
  */
  const ATimer& getAgeTimer() const;
  
  /*!
  Last used timer
  Gets reset when object is de-serialized via fromAFile

  @return Constant reference to the ATimer object
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
  //! Reference to the container holding this data
  AOSSessionMapHolder& m_Holder;
  
  //! Session data
  AXmlDocument m_Data;
  
  //! Session life timer
  ATimer m_AgeTimer;
  
  //! Session last used timer
  ATimer m_LastUsedTimer;
  
  //! Data sync object
  ASynchronization *mp_SyncObject;
};

#endif //INCLUDED__AOSSessionData_HPP__
