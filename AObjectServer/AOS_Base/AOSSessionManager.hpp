#ifndef INCLUDED__AOSSessionManager_HPP__
#define INCLUDED__AOSSessionManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSSessionData.hpp"
#include "ASynchronization.hpp"

class AOSServices;

#define HASHMAP_SIZE 0x10

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
  typedef std::map<AString, AOSSessionData *> SESSION_MAP;
  class SessionMapHolder
  {
  public:
    SessionMapHolder(ASynchronization *pSynch) : mp_SynchObject(pSynch) {}
    ~SessionMapHolder() { delete(mp_SynchObject); }
    SESSION_MAP m_SessionMap;
    ASynchronization *mp_SynchObject;
  };
  
  typedef std::map<size_t, SessionMapHolder *> SESSION_HASHMAP;

  SESSION_HASHMAP m_SessionHashMap;

  AOSSessionManager::SessionMapHolder *_getSessionHolder(const AString&);

  AOSServices& m_Services;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSSessionManager_HPP__
