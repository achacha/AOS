/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSSessionManager_HPP__
#define INCLUDED__AOSSessionManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSSessionData.hpp"
#include "AOSSessionMapHolder.hpp"
#include "AThread.hpp"

class AOSServices;
class AOSContext;

class AOS_BASE_API AOSSessionManager : public AOSAdminInterface
{
public:
  //! Class name
  static const AString CLASS;

  //! Name of the cookie that stores session id
  static const AString SESSIONID;

public:
  //! ctor
  AOSSessionManager(AOSServices&);
  
  //! dtor
  virtual ~AOSSessionManager();
  
  /*!
  Initalize session
  If session id in request cookie, fetch session and associate it with context
  If invalid session id or not there , create session, add response cookie and associate it with the corrent context

  @param context current
  */
  void initOrCreateSession(AOSContext& context);

  /*!
  Finalize the session with respect to the context

  @param context current
  */
  void finalizeSession(AOSContext& context);

  /*!
  Check existance of session data
  If session has already expired it will be removed from database and reported as non-existing

  @param sessionId checks if session exists
  @return true if session exists
  */
  bool exists(const AString& sessionId);

  /*!
  Generates (and appends) a session id string (somewhat unique)
  
  @param sessionId appended to output buffer
  */
  void generateSessionId(AOutputBuffer& sessionId) const;

  /*!
  Adds session id to the response for lazy create of data later
  */
  void addSessionId(AOSContext& context);
  
  /*!
  Create a new session data and populate new session id
  
  @param sessionId string to receive new session id, will be cleared first
  @return new session data object
  */
  AOSSessionData *createNewSessionData(AString& sessionId);

  /*!
  Get session data
  
  @param sessionId for existing session, will return NULL if not found or expired
  @return existing session data object
  */
  AOSSessionData *getSessionData(const AString& sessionId);

  /*!
  Persist session data to the database
  If database persistence is enabled it will save the session data
  When exists/getSessionData is called it will restore it back into memory

  @param sessionId for existing session to be persisted
  */
  void persistSession(const AString& sessionId);
  void persistSession(AOSSessionData *pData);

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Holder for session maps
  typedef std::vector<AOSSessionMapHolder *> SESSION_MAPHOLDER_CONTAINER;
  SESSION_MAPHOLDER_CONTAINER m_SessionHolderContainer;
 
  AOSSessionMapHolder *_getSessionHolder(const AString&);

  //!Reference to services
  AOSServices& m_Services;

  //! Size of vector for session holder
  size_t m_HolderSize;

  //! Session timeout
  //! Time in milliseconds to coincide with Timer.getInterval()
  double m_TimeoutInterval;
  long m_TimeoutIntervalInSeconds;

  //! Sleep cycle time in milliseconds
  u4 m_SessionMonitorSleep;

  bool m_DatabasePersistence;

  //! threadproc that cleans up old sessions
  static u4 threadprocSessionManager(AThread&);

  //! Manager thread
  AThread m_Thread;

  //! Add response cookie for session id
  void _addSessionIdCookie(AOSContext& context, const AString& sessionId);
  
  //! Database restore
  //! returns NULL if not found or expired
  AOSSessionData *_restoreSession(const AString& sessionId);
};

#endif //INCLUDED__AOSSessionManager_HPP__
