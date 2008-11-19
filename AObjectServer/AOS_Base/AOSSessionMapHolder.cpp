/*
Written by Alex Chachanashvili

$Id: AOSSessionMapHolder.cpp 252 2008-08-02 21:26:15Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSSessionMapHolder.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AFile_AString.hpp"

void AOSSessionMapHolder::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  for (CONTAINER::const_iterator cit = m_SessionMap.begin(); cit != m_SessionMap.end(); ++cit)
    (*cit).second->debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSSessionMapHolder::AOSSessionMapHolder(ASynchronization *pSynch) :
  mp_SyncObject(pSynch)  
{
}

AOSSessionMapHolder::~AOSSessionMapHolder()
{
  try
  {
    {
      ALock lock(mp_SyncObject);
      for (CONTAINER::iterator it = m_SessionMap.begin(); it != m_SessionMap.end(); ++it)
      {
        delete (*it).second;
      }
    }
    delete(mp_SyncObject);
  }
  catch(...){}
}

AOSSessionMapHolder::CONTAINER AOSSessionMapHolder::useContainer()
{
  return m_SessionMap;
}

ASynchronization& AOSSessionMapHolder::useSyncObject()
{
  AASSERT(this, mp_SyncObject);
  return *mp_SyncObject;
}

bool AOSSessionMapHolder::exists(const AString& sessionId)
{
  //a_Find session in the bucket
  return (m_SessionMap.end() != m_SessionMap.find(sessionId));
}

AOSSessionData *AOSSessionMapHolder::getOrCreateSessionData(const AString& sessionId)
{
  //a_Find session in the bucket
  CONTAINER::iterator it = m_SessionMap.find(sessionId);
  if (it != m_SessionMap.end())
  {
    ALock lock(mp_SyncObject);
    (*it).second->restartLastUsedTimer();        //a_Restart session timer
    return ((*it).second);
  }
  else
  {
    //a_Not found, create new one
    AOSSessionData *pData = NULL;    
    ALock lock(mp_SyncObject);
    
    //a_Create a new session
    pData = new AOSSessionData(sessionId, *this);
    m_SessionMap.insert(CONTAINER::value_type(sessionId, pData));
    return pData;
  }
}

void AOSSessionMapHolder::purgeExpired(double timeoutInterval)
{
  ALock lock(mp_SyncObject);
  CONTAINER::iterator it = m_SessionMap.begin(); 
  while (it != m_SessionMap.end())
  {
    AOSSessionData *pData = it->second;
    if (pData->getLastUsedTimer().getInterval() > timeoutInterval)
    {
      //a_Timed out session
      CONTAINER::iterator itKill = it;
      ++it;
      m_SessionMap.erase(itKill);
      delete pData;
    }
    else
    {
      ++it;
    }
  }
}

void AOSSessionMapHolder::setSessionData(const AString& sessionId, const AString& data)
{
  ALock lock(mp_SyncObject);
  
  //a_Create a new session
  AOSSessionData *pData = new AOSSessionData(sessionId, *this);
  AFile_AString datafile(data);
  pData->fromAFile(datafile);
  m_SessionMap.insert(CONTAINER::value_type(sessionId, pData));
}
