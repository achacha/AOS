#include "pchABase.hpp"
#include "ADatabasePool.hpp"
#include "ASystemException.hpp"
#include "ALock.hpp"

void ADatabasePool::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ADatabasePool @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_FreeDatabases.size()=" << m_FreeDatabases.size() << std::endl;
  DATABASES::const_iterator cit = m_FreeDatabases.begin();
  while (cit != m_FreeDatabases.end())
  {
    ADebugDumpable::indent(os, indent+1) << "{" << std::endl;
    (*cit)->m_inUseTimer.debugDump(os, indent+2);
    (*cit)->m_averageTimeInUse.debugDump(os, indent+2);
    (*cit)->mp_Database->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
    ++cit;
  }
  
  ADebugDumpable::indent(os, indent+1) << "m_LockedDatabases.size()=" << m_LockedDatabases.size() << std::endl;
  cit = m_LockedDatabases.begin();
  while (cit != m_LockedDatabases.end())
  {
    ADebugDumpable::indent(os, indent+1) << "{" << std::endl;
    (*cit)->m_inUseTimer.debugDump(os, indent+2);
    (*cit)->m_averageTimeInUse.debugDump(os, indent+2);
    (*cit)->mp_Database->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ADatabasePool::ADatabasePool(
  ADatabase *pDatabase,
  int maxConnections    // = 6
) :
  m_Semaphore(pDatabase->getUrl().toAString(), maxConnections)
{
  AString error;

  //a_Initialize first one (the template)
  if (!pDatabase->isInitialized())
    if (!pDatabase->init(error))
      ATHROW_EX(this, AException::InitializationFailure, error);
  
  //a_Create additional clones so the total is maxConnections
  m_FreeDatabases.push_back(new ADatabaseHolder(pDatabase));
  for (int i=1; i<maxConnections; ++i)
  {
    ADatabase *pClone = pDatabase->clone(error);
    if (!pClone)
      ATHROW_EX(this, AException::InitializationFailure, error);
    m_FreeDatabases.push_back(new ADatabaseHolder(pClone));
  }
}

ADatabasePool::~ADatabasePool()
{
  try
  {
    DATABASES::iterator it = m_FreeDatabases.begin();
    while (it != m_FreeDatabases.end())
    {
      (*it)->mp_Database->deinit();
      pDelete((*it)->mp_Database);
      pDelete(*it);
      ++it;
    }
    
    it = m_LockedDatabases.begin();
    while (it != m_LockedDatabases.end())
    {
      (*it)->mp_Database->deinit();
      pDelete((*it)->mp_Database);
      pDelete(*it);
      ++it;
    }
  }
  catch(...) {}
}

void ADatabasePool::add(ADatabase *pDatabase)
{
  AASSERT(this, pDatabase);
  if (pDatabase)
  {
    m_FreeDatabases.push_back(new ADatabaseHolder(pDatabase));
  }
}

size_t ADatabasePool::executeSQL(const AString& query, AResultSet& rs, AString& error)
{
  ADatabase *pDatabase = _lockDatabase();
  AASSERT(this, NULL != pDatabase);
  size_t ret = pDatabase->executeSQL(query, rs, error);
  _unlockDatabase(pDatabase);

  return ret;
}

ADatabase *ADatabasePool::_lockDatabase()
{
  m_Semaphore.lock();

  ADatabaseHolder *pDatabaseHolder = NULL;
  {
    ALock lock(m_SynchObject);
    AASSERT(this, m_FreeDatabases.size() > 0);
    pDatabaseHolder = m_FreeDatabases.front();
    AASSERT(this, NULL != pDatabaseHolder);

    //a_Remove from free
    m_FreeDatabases.pop_front();
    
    //a_Add to locked
    m_LockedDatabases.push_back(pDatabaseHolder);
  }

  //a_Start timer
  pDatabaseHolder->m_inUseTimer.start();
    
  //a_Return ADatabase *
  AASSERT(this, NULL != pDatabaseHolder->mp_Database);
  return pDatabaseHolder->mp_Database;
}

void ADatabasePool::_unlockDatabase(ADatabase *pDatabase)
{
  ALock lock(m_SynchObject);
  DATABASES::iterator it = m_LockedDatabases.begin();
  while (it != m_LockedDatabases.end())
  {
    if ((*it)->mp_Database == pDatabase)
    {
      //a_Found our database
      ADatabaseHolder *pDatabaseHolder = *it;
      
      //a_Log time and clear timer
      pDatabaseHolder->m_inUseTimer.stop();
      pDatabaseHolder->m_averageTimeInUse.addSample(pDatabaseHolder->m_inUseTimer.getInterval());
      pDatabaseHolder->m_inUseTimer.clear();
      
      //a_Remove from locked and move to free
      m_LockedDatabases.erase(it);
      m_FreeDatabases.push_back(pDatabaseHolder);

      //a_Unlock semaphore
      m_Semaphore.unlock();

      return;
    }
    ++it;
  }
  ATHROW_EX(this, AException::InvalidObject, ASWNL("Unable to find a locked database holder"));
}
