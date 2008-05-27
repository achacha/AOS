/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSSessionManager_HPP__
#define INCLUDED__AOSSessionManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSSessionData.hpp"
#include "ASynchronization.hpp"
#include "AThread.hpp"

class AOSServices;

class AOS_BASE_API AOSSessionManager : public AOSAdminInterface
{
public:
  //! ctor
  AOSSessionManager(AOSServices&);
  
  //! dtor
  virtual ~AOSSessionManager();
  
  /*!
  Check existance of session data
  If session has already expired it will be removed from database and reported as non-existing

  @param sessionId checks if session exists
  */
  bool exists(const AString& sessionId);

  /*!
  Create a new session data and populate new session id
  
  @param sessionId string to receive new session id, will be cleared first
  @return new session data object
  */
  AOSSessionData *AOSSessionManager::createNewSessionData(AString& sessionId);

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
  //! Map of session id to session data
  typedef std::map<AString, AOSSessionData *> SESSION_MAP;
  class SessionMapHolder
  {
  public:
    SessionMapHolder(ASynchronization *pSynch) : mp_SynchObject(pSynch) {}
    ~SessionMapHolder() { delete(mp_SynchObject); }
    SESSION_MAP m_SessionMap;
    ASynchronization *mp_SynchObject;
  };
  
  //! Holder for session maps
  typedef std::vector<SessionMapHolder *> SESSION_MAPHOLDER_CONTAINER;
  SESSION_MAPHOLDER_CONTAINER m_SessionHolderContainer;
 
  AOSSessionManager::SessionMapHolder *_getSessionHolder(const AString&);

  //!Reference to services
  AOSServices& m_Services;

  //! Size of vector for session holder
  size_t m_HolderSize;

  //! Session timeout
  //! Time in milliseconds to coincide with Timer.getInterval()
  double m_TimeoutInterval;

  //! Sleep cycle time in milliseconds
  u4 m_SessionMonitorSleep;

  bool m_DatabasePersistence;

  //! threadproc that cleans up old sessions
  static u4 threadprocSessionManager(AThread&);

  //! Manager thread
  AThread m_Thread;

  //! Database restore
  //! returns NULL if not found or expired
  AOSSessionData *_restoreSession(const AString& sessionId);
};

#endif //INCLUDED__AOSSessionManager_HPP__
