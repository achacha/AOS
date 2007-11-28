#include "pchAOS_Base.hpp"
#include "AOSSessionManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"

#ifdef __DEBUG_DUMP__
void AOSSessionManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSSessionManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_SessionHashMap={" << std::endl;
  SESSION_HASHMAP::const_iterator citHashMap = m_SessionHashMap.begin();
  while(citHashMap != m_SessionHashMap.end())
  {
    SessionMapHolder *pSessionMapHolder = citHashMap->second;
    ALock lock(pSessionMapHolder->mp_SynchObject);
    SESSION_MAP::const_iterator cit = pSessionMapHolder->m_SessionMap.begin();
    while (cit != pSessionMapHolder->m_SessionMap.end())
    {
      (*cit).second->debugDump(os, indent+2);
      ++cit;
    }
    ++citHashMap;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

const AString& AOSSessionManager::getClass() const
{
  static const AString CLASS("AOSSessionManager");
  return CLASS;
}

void AOSSessionManager::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  SESSION_HASHMAP::const_iterator citHashMap = m_SessionHashMap.begin();
  while(citHashMap != m_SessionHashMap.end())
  {
    SessionMapHolder *pSessionMapHolder = citHashMap->second;
    ALock lock(pSessionMapHolder->mp_SynchObject);
    AXmlElement& eHashMap = addProperty(eBase, ASW("SessionHashMap",14), AString::fromSize_t(pSessionMapHolder->m_SessionMap.size()));  
    eHashMap.addAttribute(ASW("hash", 4), AString::fromSize_t(citHashMap->first));
    eHashMap.addElement(ASW("size", 4), AString::fromSize_t(pSessionMapHolder->m_SessionMap.size()));
    ++citHashMap;
  }
}

AOSSessionManager::AOSSessionManager(AOSServices& services) :
  m_Services(services)
{
  registerAdminObject(m_Services.useAdminRegistry());
}

AOSSessionManager::~AOSSessionManager()
{
  SESSION_HASHMAP::iterator itHashMap = m_SessionHashMap.begin();
  while(itHashMap != m_SessionHashMap.end())
  {
    SessionMapHolder *pSessionMapHolder = (*itHashMap).second;
    {
      ALock lock(pSessionMapHolder->mp_SynchObject);
      SESSION_MAP::iterator it = pSessionMapHolder->m_SessionMap.begin();
      while (it != pSessionMapHolder->m_SessionMap.end())
      {
        pDelete((*it).second);
        ++it;
      }
    }
    delete(pSessionMapHolder);
    ++itHashMap;
  }
}

bool AOSSessionManager::exists(const AString& sessionId)
{
  SessionMapHolder *pSessionMapHolder = _getSessionHolder(sessionId);
  AASSERT(this, pSessionMapHolder);
  
  //a_Find session in the bucket
  return (pSessionMapHolder->m_SessionMap.end() != pSessionMapHolder->m_SessionMap.find(sessionId));
}

AOSSessionData *AOSSessionManager::getSessionData(const AString& sessionId)
{
  SessionMapHolder *pSessionMapHolder = _getSessionHolder(sessionId);
  AASSERT(this, pSessionMapHolder);
  
  //a_Find session in the bucket
  SESSION_MAP::iterator it = pSessionMapHolder->m_SessionMap.find(sessionId);
  if (it != pSessionMapHolder->m_SessionMap.end())
  {
    (*it).second->restartLastUsedTimer();        //a_Restart session timer
    return ((*it).second);
  }
  else
  {
    ALock lock(pSessionMapHolder->mp_SynchObject);
    
    //a_Create a new session
    AOSSessionData *pData = new AOSSessionData(sessionId);
    pSessionMapHolder->m_SessionMap.insert(SESSION_MAP::value_type(sessionId, pData));
    return pData;
  }
}

AOSSessionManager::SessionMapHolder *AOSSessionManager::_getSessionHolder(const AString& sessionId)
{
  size_t hash = sessionId.getHash(HASHMAP_SIZE);
  SESSION_HASHMAP::iterator itHashMap = m_SessionHashMap.find(hash);
  if (itHashMap == m_SessionHashMap.end())
  {
    //a_Create session and the lock
    SessionMapHolder *pSessionMapHolder = new SessionMapHolder(new ASync_CriticalSection());
    m_SessionHashMap[hash] = pSessionMapHolder;
    return pSessionMapHolder;
  }
  else
  {
    //a_Use existing
    return (*itHashMap).second;
  }
}
