#ifndef INCLUDED__AOSSessionManager_HPP__
#define INCLUDED__AOSSessionManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSSessionData.hpp"
#include "ASynchronization.hpp"
#include "AThread.hpp"

class AOSServices;

#define DEFAULT_HOLDER_SIZE 13

class AOS_BASE_API AOSSessionManager : public AOSAdminInterface
{
public:
  AOSSessionManager(AOSServices&);
  virtual ~AOSSessionManager();
  
  /*!
  Check existance of session data
  */
  bool exists(const AString& sessionId);

  /*!
  Get session data
  */
  AOSSessionData *getSessionData(const AString& sessionId);

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

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

  //! threadproc that cleans up old sessions
  static u4 threadprocSessionManager(AThread&);

  //! Manager thread
  AThread m_Thread;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSSessionManager_HPP__
