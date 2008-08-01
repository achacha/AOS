/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSSessionManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSContext.hpp"
#include "AThread.hpp"
#include "AResultSet.hpp"
#include "AFile_AString.hpp"

#define DEFAULT_HOLDER_SIZE 13
#define DEFAULT_SLEEP_DURATION 3000

const AString AOSSessionManager::SESSIONID("AOSSession", 10);

void AOSSessionManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

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

const AString& AOSSessionManager::getClass() const
{
  static const AString CLASS("AOSSessionManager");
  return CLASS;
}

void AOSSessionManager::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  adminAddProperty(
    eBase,
    ASW("timeout",7),
    AString::fromDouble(m_TimeoutInterval)
  );

  adminAddProperty(
    eBase,
    ASW("sleep-cycle",11),
    AString::fromU4(m_SessionMonitorSleep)
  );

  for (size_t i=0; i<m_HolderSize; ++i)
  {
    SessionMapHolder *pSessionMapHolder = m_SessionHolderContainer[i];
    ALock lock(pSessionMapHolder->mp_SynchObject);
    AXmlElement& eHashMap = adminAddProperty(eBase, ASW("SessionMap",10), AString::fromSize_t(pSessionMapHolder->m_SessionMap.size()));  
    eHashMap.addAttribute(ASW("hash", 4), AString::fromSize_t(i));
    eHashMap.addElement(ASW("size", 4), AString::fromSize_t(pSessionMapHolder->m_SessionMap.size()));
  }
}

AOSSessionManager::AOSSessionManager(AOSServices& services) :
  m_Services(services),
  m_Thread(AOSSessionManager::threadprocSessionManager)
{
  adminRegisterObject(m_Services.useAdminRegistry());

  //a_Read config
  m_TimeoutInterval = (double)m_Services.useConfiguration().useConfigRoot().getU4("/config/server/session-manager/timeout", 360000);
  m_TimeoutIntervalInSeconds = m_TimeoutInterval / 1000;
  AASSERT(this, m_TimeoutInterval > 0.0);
  m_SessionMonitorSleep = m_Services.useConfiguration().useConfigRoot().getU4("/config/server/session-manager/sleep-cycle", DEFAULT_SLEEP_DURATION);
  AASSERT(this, m_SessionMonitorSleep > 0);
  m_HolderSize = m_Services.useConfiguration().useConfigRoot().getSize_t("/config/server/session-manager/holder-size", DEFAULT_HOLDER_SIZE);
  AASSERT(this, m_HolderSize > 0);
  m_DatabasePersistence = m_Services.useConfiguration().useConfigRoot().getBool("/config/server/session-manager/database-persistence/enabled", false);

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
  bool isFound = (pSessionMapHolder->m_SessionMap.end() != pSessionMapHolder->m_SessionMap.find(sessionId));
  if (
    !isFound
    && m_DatabasePersistence)
  {
    //a_Try database
    isFound = (NULL != _restoreSession(sessionId));
  }

  return isFound;
}

AOSSessionData *AOSSessionManager::getSessionData(const AString& sessionId)
{
  SessionMapHolder *pSessionMapHolder = _getSessionHolder(sessionId);
  AASSERT(this, pSessionMapHolder);
  
  //a_Find session in the bucket
  SESSION_MAP::iterator it = pSessionMapHolder->m_SessionMap.find(sessionId);
  if (it != pSessionMapHolder->m_SessionMap.end())
  {
    ALock lock(pSessionMapHolder->mp_SynchObject);
    (*it).second->restartLastUsedTimer();        //a_Restart session timer
    return ((*it).second);
  }
  else
  {
    //a_Not found, create new one
    AOSSessionData *pData = NULL;    
    ALock lock(pSessionMapHolder->mp_SynchObject);
    
    //a_Create a new session
    pData = new AOSSessionData(sessionId);
    pSessionMapHolder->m_SessionMap.insert(SESSION_MAP::value_type(sessionId, pData));
    return pData;
  }
}

AOSSessionData *AOSSessionManager::createNewSessionData(AString& sessionId)
{
  sessionId.clear();
  AString localHostHash(AString::fromSize_t(ASocketLibrary::getLocalHostName().getHash()));
  ATextGenerator::generateRandomAlphanum(sessionId, 13);
  sessionId.append(AString::fromSize_t(ATime::getTickCount()));
  sessionId.append(localHostHash);

  SessionMapHolder *pSessionMapHolder = _getSessionHolder(sessionId);
  AASSERT(this, pSessionMapHolder);

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

void AOSSessionManager::persistSession(const AString& sessionId)
{
  AOSSessionData *pData = getSessionData(sessionId);
  if (!pData)
  {
    m_Services.useLog().add(ARope("Trying to persist non-existing session: ")+sessionId, ALog::WARNING);
    return;
  }
  persistSession(pData);
}

void AOSSessionManager::persistSession(AOSSessionData *pData)
{
  AASSERT(this, pData);

  //a_First check if it already exists
  AString query("SELECT id FROM session WHERE id='");
  AString sessionId;
  if (!pData->getSessionId(sessionId))
  {
    ATHROW_EX(pData, AException::InvalidData, ASWNL("Session data cannot persist without a valid session id"));
    return;
  }
  query.append(sessionId);
  query.append("'",1);

  AResultSet result;
  AString strError;
  size_t rows = m_Services.useDatabaseConnectionPool().useDatabasePool().executeSQL(query, result, strError);
  if (!strError.isEmpty())
  {
    m_Services.useLog().add(strError, ALog::FAILURE);
    return;
  }

  AFile_AString datafile;
  pData->toAFile(datafile);
  
  //a_No point in persisting empty data
  if (datafile.useAString().isEmpty())
    return;

  if (rows > 0)
  {
    //a_UPDATE
    query.assign("UPDATE session SET data='");
    query.append(datafile);
    query.append("' WHERE id='");
    query.append(sessionId);
    query.append("'",1);

    result.clear();
    m_Services.useDatabaseConnectionPool().useDatabasePool().executeSQL(query, result, strError);
    if (!strError.isEmpty())
    {
      m_Services.useLog().add(strError, ALog::FAILURE);
    }
  }           
  else
  {
    //a_INSERT
    query.assign("INSERT INTO session(id,data) VALUES ('");
    query.append(sessionId);
    query.append("','");
    query.append(datafile);
    query.append("')");
    result.clear();
    m_Services.useDatabaseConnectionPool().useDatabasePool().executeSQL(query, result, strError);
    if (!strError.isEmpty())
    {
      m_Services.useLog().add(strError, ALog::FAILURE);
    }
  }
}

AOSSessionData *AOSSessionManager::_restoreSession(const AString& sessionId)
{
  if (sessionId.isEmpty())
    return NULL;

  AString query("SELECT data FROM session WHERE id='");
  query.append(sessionId);
  query.append("'",1);

  AResultSet result;
  AString strError;
  size_t rows = m_Services.useDatabaseConnectionPool().useDatabasePool().executeSQL(query, result, strError);
  if (!strError.isEmpty())
  {
    m_Services.useLog().add(strError, ALog::FAILURE);
    return NULL;
  }

  AOSSessionData *pData = NULL;
  if (rows > 0)
  {
    result.getData(0,0);

    SessionMapHolder *pSessionMapHolder = _getSessionHolder(sessionId);
    AASSERT(this, pSessionMapHolder);
    ALock lock(pSessionMapHolder->mp_SynchObject);
    
    //a_Create a new session
    pData = new AOSSessionData(sessionId);
    AFile_AString datafile(result.getData(0,0));
    pData->fromAFile(datafile);
    pSessionMapHolder->m_SessionMap.insert(SESSION_MAP::value_type(sessionId, pData));
  }

  return pData;
}

void AOSSessionManager::initSession(AOSContext& context)
{
  //a_Create/Use session cookie/data
  AString sessionId;
  context.useRequestCookies().getValue(AOSSessionManager::SESSIONID, sessionId);
  if (!sessionId.isEmpty() && m_Services.useSessionManager().exists(sessionId))
  {
    //a_Fetch session
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
      context.useEventVisitor().startEvent(AString("Using existing session ",23)+sessionId, AEventVisitor::EL_INFO);
    
    AOSSessionData *pSessionData = getSessionData(sessionId);
    context.setSessionObject(pSessionData);
  }
  else
  {
    //a_Create session
    if (!sessionId.isEmpty())
    {
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
      {
        ARope rope("Creating new session, existing session not found: ",50);
        rope.append(sessionId);
        context.useEventVisitor().startEvent(rope, AEventVisitor::EL_INFO);
      }
      sessionId.clear();
    }
    
    //a_Craete a new session
    AOSSessionData *pSessionData = createNewSessionData(sessionId);

    //a_This call will populate sessionId and return the new session data object
    context.setSessionObject(pSessionData);

    //a_Add cookie for this session
    ACookie& cookie = context.useResponseCookies().addCookie(AOSSessionManager::SESSIONID, sessionId);
    cookie.setMaxAge(m_TimeoutIntervalInSeconds);
    cookie.setPath(ASW("/",1));
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
    {
      AString str("Created new session ",20);
      str.append(sessionId);
      context.useEventVisitor().startEvent(str, AEventVisitor::EL_INFO);
    }
  }
}

void AOSSessionManager::finalizeSession(AOSContext& context)
{
  AString sessionId;
  AVERIFY(&context, context.useRequestCookies().getValue(AOSSessionManager::SESSIONID, sessionId));
  SessionMapHolder *pSessionMapHolder = _getSessionHolder(sessionId);
  AASSERT(this, pSessionMapHolder);

  {
    ALock lock(pSessionMapHolder->mp_SynchObject);
    AOSSessionData *pSessionData = context.setSessionObject(NULL);
    AASSERT(this, pSessionData);
    pSessionData->finalize(context);
  }
}
