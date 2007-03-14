#include "pchAOS_Base.hpp"
#include "AOSServices.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSConfiguration.hpp"
#include "AMutex.hpp"
#include "AResultSet.hpp"

#ifdef __DEBUG_DUMP__
void AOSServices::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSServices @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Log=" << std::endl;
  m_Log.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_AdminRegistry=" << std::endl;
  m_AdminRegistry.debugDump(os, indent+2);

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
  
  addProperty(eBase, ASW("log",3), m_Log);
  
  {
    AString str(16,16);
    str.parseU4(m_Log.getEventMask(), 16);
    str.justifyRight(8, '0');
    addProperty(eBase, ASW("log.eventmask",13), str);
  }
}

AOSServices::AOSServices(const AFilename& iniFilename) :
  m_ConsoleSynch("Console_cout"),
  m_Log(new AMutex(iniFilename.toAString()), iniFilename.toAString()+".log"),
  m_AdminRegistry(m_Log),
  m_GlobalObjects(ASW("global",6)),
  mp_Configuration(NULL),
  mp_DatabaseConnPool(NULL),
  mp_SessionManager(NULL),
  mp_ContextManager(NULL)
{
  mp_Configuration = new AOSConfiguration(iniFilename, *this, m_ConsoleSynch);

  _initDatabasePool();

  mp_SessionManager = new AOSSessionManager(*this);
  mp_ContextManager = new AOSContextManager(*this);
}

AOSServices::AOSServices(const AFilename& iniFilename, ALog::EVENT_MASK mask) :
  m_ConsoleSynch("Console_cout"),
  m_Log(new AMutex(iniFilename.toAString()), iniFilename.toAString()+".log", mask),
  mp_Configuration(NULL),
  mp_DatabaseConnPool(NULL),
  m_AdminRegistry(m_Log),
  m_GlobalObjects(ASW("global",6)),
  mp_SessionManager(NULL),
  mp_ContextManager(NULL)
{
  mp_Configuration = new AOSConfiguration(iniFilename, *this, m_ConsoleSynch);

  _initDatabasePool();

  mp_SessionManager = new AOSSessionManager(*this);
  mp_ContextManager = new AOSContextManager(*this);
}

AOSServices::~AOSServices()
{
  delete mp_SessionManager;
  delete mp_ContextManager;
  delete mp_DatabaseConnPool;
  delete mp_Configuration;
}

void AOSServices::_initDatabasePool()
{
  mp_DatabaseConnPool = new AOSDatabaseConnectionPool(*this);

  AUrl urlServer;
  int iMaxConnections = mp_Configuration->getDatabaseMaxConnections();
  if (iMaxConnections > 0 && mp_Configuration->getDatabaseUrl(urlServer))
  {
    mp_DatabaseConnPool->init(urlServer, iMaxConnections);
  }
}

ALog& AOSServices::useLog()
{
  return m_Log;
}

AOSConfiguration& AOSServices::useConfiguration()
{
  return *mp_Configuration;
}

AOSAdminRegistry& AOSServices::useAdminRegistry()
{
  return m_AdminRegistry;
}

AOSDatabaseConnectionPool& AOSServices::useDatabaseConnectionPool()
{
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
