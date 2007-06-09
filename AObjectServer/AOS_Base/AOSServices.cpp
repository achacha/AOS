#include "pchAOS_Base.hpp"
#include "AOSServices.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSConfiguration.hpp"
#include "ASync_Mutex.hpp"
#include "AResultSet.hpp"

#ifdef __DEBUG_DUMP__
void AOSServices::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSServices @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Log=" << std::endl;
  mp_Log->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_AdminRegistry=" << std::endl;
  mp_AdminRegistry->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_DatabaseConnPool=" << std::endl;
  mp_DatabaseConnPool->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_SessionManager=" << std::endl;
  mp_SessionManager->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_ContextManager=" << std::endl;
  mp_ContextManager->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_Configuration=" << std::endl;
  mp_Configuration->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_GlobalObjects=" << std::endl;
  m_GlobalObjects.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

const AString& AOSServices::getClass() const
{
  static const AString CLASS("AOSServices");
  return CLASS;
}

void AOSServices::registerAdminObject(AOSAdminRegistry& registry)
{
  registry.insert(getClass(), *this);
}

void AOSServices::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);
  
  addProperty(eBase, ASW("log",3), *mp_Log);
  
  {
    AString str(16,16);
    str.parseU4(mp_Log->getEventMask(), 16);
    str.justifyRight(8, '0');
    addProperty(eBase, ASW("log.eventmask",13), str);
  }
}

AOSServices::AOSServices(const AFilename& basePath, ALog::EVENT_MASK mask) :
  m_ConsoleSynch("Console_cout"),
  m_GlobalObjects(ASW("global",6)),
  mp_Configuration(NULL),
  mp_DatabaseConnPool(NULL),
  mp_SessionManager(NULL),
  mp_ContextManager(NULL),
  mp_CacheManager(NULL),
  mp_Log(NULL)
{
  //a_Create log
  AFilename logfile(basePath);
  logfile.usePathNames().push_back(ASW("logs",4));
  logfile.useFilename().assign(ASW("aos.log",7));
  AFilename mutexfile(logfile);
  mutexfile.useFilename().assign(ASW("aos.mutex",9));
  mp_Log = new ALog_AFile(new ASync_Mutex(mutexfile.toAString()), logfile, mask);

  mp_AdminRegistry = new AOSAdminRegistry(*mp_Log);
  mp_Configuration = new AOSConfiguration(basePath, *this, m_ConsoleSynch);
  mp_SessionManager = new AOSSessionManager(*this);
  mp_ContextManager = new AOSContextManager(*this);
  mp_CacheManager = new AOSCacheManager(*this);
}

AOSServices::~AOSServices()
{
  try
  {
    delete mp_AdminRegistry;
    delete mp_SessionManager;
    delete mp_ContextManager;
    delete mp_CacheManager;
    delete mp_DatabaseConnPool;
    delete mp_Configuration;
    delete mp_Log;
  }
  catch(...) {}
}

bool AOSServices::initDatabasePool()
{
  mp_DatabaseConnPool = new AOSDatabaseConnectionPool(*this);

  int iMaxConnections = mp_Configuration->getInt(AOSConfiguration::DATABASE_CONNECTIONS, 2);
  AString strUrl = mp_Configuration->getString(AOSConfiguration::DATABASE_URL, AString::sstr_Empty);
  if (iMaxConnections > 0 && !strUrl.isEmpty())
  {
    AUrl urlServer(strUrl);
    mp_DatabaseConnPool->init(urlServer, iMaxConnections);
    return true;
  }
  else
  {
    mp_Log->add(ARope("AOSServices::_initDatabasePool: Unable to init DB pool url='")+strUrl+ASWNL("' connections=")+AString::fromInt(iMaxConnections) , ALog::FAILURE);
    return false;
  }
}

ALog& AOSServices::useLog()
{
  AASSERT(this, mp_Log);
  return *mp_Log;
}

AOSConfiguration& AOSServices::useConfiguration()
{
  return *mp_Configuration;
}

AOSAdminRegistry& AOSServices::useAdminRegistry()
{
  AASSERT(this, mp_AdminRegistry);
  return *mp_AdminRegistry;
}

AOSDatabaseConnectionPool& AOSServices::useDatabaseConnectionPool()
{
  AASSERT(this, mp_DatabaseConnPool);
  return *mp_DatabaseConnPool;
}

size_t AOSServices::loadGlobalObjects(AString& strError)
{
  AResultSet resultSet;
  if (mp_DatabaseConnPool->useDatabasePool().executeSQL("select * from global", resultSet, strError))
  {
    if (resultSet.getRowCount() > 0)
    {
      size_t nName = resultSet.getFieldIndex(ASW("name", 4));
      size_t nValue = resultSet.getFieldIndex(ASW("value", 5));
      for (size_t u=0; u<resultSet.getRowCount(); ++u)
      {
        m_GlobalObjects.insert(resultSet.getData(u, nName), resultSet.getData(u, nValue), AXmlData::CDataSafe);
      }
    }
  }
  else
    return AConstant::npos;

  return resultSet.getRowCount();
}

AObjectContainer& AOSServices::useGlobalObjects()
{ 
  return m_GlobalObjects;
}

AOSSessionManager& AOSServices::useSessionManager()
{
  return *mp_SessionManager;
}         

AOSContextManager& AOSServices::useContextManager()
{
  return *mp_ContextManager;
}

AOSCacheManager& AOSServices::useCacheManager()
{
  return *mp_CacheManager;
}
