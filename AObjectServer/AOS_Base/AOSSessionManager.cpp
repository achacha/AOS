#include "pchAOS_Base.hpp"
#include "AOSSessionManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AThread.hpp"

#define DEFAULT_SLEEP_DURATION 3000

#ifdef __DEBUG_DUMP__
void AOSSessionManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSSessionManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_TimeoutInterval=" << m_TimeoutInterval << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_SessionMonitorSleep=" << m_SessionMonitorSleep << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_SessionHolderContainer={" << std::endl;
  for (size_t i=0; i<m_HolderSize; ++i)
  {
    SessionMapHolder *pSessionMapHolder = m_SessionHolderContainer[i];
    ALock lock(pSessionMapHolder->mp_SynchObject);
    SESSION_MAP::const_iterator cit = pSessionMapHolder->m_SessionMap.begin();
    while (cit != pSessionMapHolder->m_SessionMap.end())
    {
      (*cit).second->debugDump(os, indent+2);
      ++cit;
    }
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

  addProperty(
    eBase,
    ASW("timeout",7),
    AString::fromDouble(m_TimeoutInterval)
  );

  addProperty(
    eBase,
    ASW("sleep-cycle",11),
    AString::fromU4(m_SessionMonitorSleep)
  );

  for (size_t i=0; i<m_HolderSize; ++i)
  {
    SessionMapHolder *pSessionMapHolder = m_SessionHolderContainer[i];
    ALock lock(pSessionMapHolder->mp_SynchObject);
    AXmlElement& eHashMap = addProperty(eBase, ASW("SessionMap",10), AString::fromSize_t(pSessionMapHolder->m_SessionMap.size()));  
    eHashMap.addAttribute(ASW("hash", 4), AString::fromSize_t(i));
    eHashMap.addElement(ASW("size", 4), AString::fromSize_t(pSessionMapHolder->m_SessionMap.size()));
  }
}

AOSSessionManager::AOSSessionManager(AOSServices& services) :
  m_Services(services),
  m_Thread(AOSSessionManager::threadprocSessionManager)
{
  registerAdminObject(m_Services.useAdminRegistry());

  //a_Read config
  m_TimeoutInterval = (double)m_Services.useConfiguration().useConfigRoot().getU4("/config/server/session-manager/timeout", 360000);
  AASSERT(this, m_TimeoutInterval > 0.0);
  m_SessionMonitorSleep = m_Services.useConfiguration().useConfigRoot().getU4("/config/server/session-manager/sleep-cycle", DEFAULT_SLEEP_DURATION);
  AASSERT(this, m_SessionMonitorSleep > 0);
  m_HolderSize = m_Services.useConfiguration().useConfigRoot().getSize_t("/config/server/session-manager/holder-size", DEFAULT_HOLDER_SIZE);
  AASSERT(this, m_HolderSize > 0);


  //a_Create holders
  m_SessionHolderContainer.resize(m_HolderSize);
  for (size_t i=0; i<m_HolderSize; ++i)
    m_SessionHolderContainer[i] = new SessionMapHolder(new ASync_CriticalSection());

  //a_Start manager thread
  m_Thread.setThis(this);
  m_Thread.start();


}

AOSSessionManager::~AOSSessionManager()
{
  //a_Stop manager thread first
  m_Thread.setRun(false);
  int wait = 3;
  while (m_Thread.isRunning() && wait)
  {
    --wait;
    AThread::sleep(DEFAULT_SLEEP_DURATION);
  }
  if (m_Thread.isRunning())
    m_Thread.terminate();  //a_It must stop before cleanup

  //a_Cleanup sessions
  for (size_t i=0; i<m_HolderSize; ++i)
  {
    SessionMapHolder *pSessionMapHolder = m_SessionHolderContainer[i];
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
  }
}

u4 AOSSessionManager::threadprocSessionManager(AThread& thread)
{
  AOSSessionManager *pThis = dynamic_cast<AOSSessionManager *>(thread.getThis());
  AASSERT(&thread, pThis);

  thread.setRunning(true);  // Flag thread as starting to run

  //a_Sleep a bit first
  AThread::sleep(pThis->m_SessionMonitorSleep);

  size_t currentMap = 0;
  try
  {
    do
    {
      //Iterate next map and remove any expired sessions
      SessionMapHolder *pSessionMapHolder = pThis->m_SessionHolderContainer[currentMap];
      if (!pSessionMapHolder->m_SessionMap.empty())
      {
        ALock lock(pSessionMapHolder->mp_SynchObject);
        SESSION_MAP::iterator it = pSessionMapHolder->m_SessionMap.begin(); 
        while (it != pSessionMapHolder->m_SessionMap.end())
        {
          AOSSessionData *pData = it->second;
          if (pData->getLastUsedTimer().getInterval() > pThis->m_TimeoutInterval)
          {
            //a_Timed out session
            SESSION_MAP::iterator itKill = it;
            ++it;
            pSessionMapHolder->m_SessionMap.erase(itKill);
            delete pData;
          }
          else
          {
            ++it;
          }
        }
      }

      //a_Next map for cleaning
      currentMap = ((++currentMap) % pThis->m_HolderSize);

      //a_Sleep a bit
      AThread::sleep(pThis->m_SessionMonitorSleep);
    }
    while (thread.isRun());   // Execute in a loop while needed to run
  }
  catch(AException& ex)
  {
    pThis->m_Services.useLog().addException(ex);
  }
  catch(...)
  {
    pThis->m_Services.useLog().add(ASWNL("Unknown exception caught in AOSSessionManager monitor thread"), ALog::FAILURE);
  }
  thread.setRunning(false); // Flag thread as stopping
  
  return 0;
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
  size_t hash = sessionId.getHash(m_HolderSize);
  return m_SessionHolderContainer.at(hash);
}
