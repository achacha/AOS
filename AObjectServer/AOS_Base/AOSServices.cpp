/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSServices.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSConfiguration.hpp"
#include "ASync_Mutex.hpp"
#include "AResultSet.hpp"
#include "ATemplate.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "ATemplateNodeHandler_SESSION.hpp"

void AOSServices::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
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

  ADebugDumpable::indent(os, indent+1) << "*mp_InputExecutor=" << std::endl;
  mp_InputExecutor->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_ModuleExecutor=" << std::endl;
  mp_ModuleExecutor->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "*mp_OutputExecutor=" << std::endl;
  mp_OutputExecutor->debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent+1) << "m_GlobalObjects=" << std::endl;
  m_GlobalObjects.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

const AString& AOSServices::getClass() const
{
  static const AString CLASS("AOSServices");
  return CLASS;
}

void AOSServices::adminRegisterObject(AOSAdminRegistry& adminRegistry)
{
  adminRegistry.insert(getClass(), *this);
}

void AOSServices::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);
  
  adminAddProperty(eBase, ASW("log",3), *mp_Log);
  
  {
    AString str(16,16);
    str.parseU4(mp_Log->getEventMask(), 16);
    str.justifyRight(8, '0');
    adminAddProperty(eBase, ASW("log.eventmask",13), str);
  }

#ifdef DEBUG_TRACK_ABASE_MEMORY
  adminAddPropertyWithAction(
    eBase,
    ASWNL("clear_allocations"), 
    AConstant::ASTRING_EMPTY,
    ASW("Clear",5), 
    ASWNL("Clear current allocation tracker")
  );

  adminAddPropertyWithAction(
    eBase,
    ASWNL("view_allocations"), 
    AConstant::ASTRING_EMPTY,
    ASW("View",4), 
    ASWNL("View level: 1:memory and size  2:include hexdump"),
    ASW("level",5)
  );

  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("property",8), str))
  {
    if (str.equals(ASWNL("AOSServices.clear_allocations")))
    {
      ABase::clearAllocations();
    }
  }

  str.clear();
  if (request.getUrl().getParameterPairs().get(ASW("property",8), str))
  {
    if (str.equals(ASWNL("AOSServices.view_allocations")))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("level",5), str))
      {
        int level = str.toInt();
        if (level > 0)
        {
          ARope target(8192);
          traceAllocations(target, level > 1);
          adminAddProperty(eBase, ASW("allocations",11), target, AXmlElement::ENC_CDATADIRECT);
        }
      }
    }
  }
#endif
}

void AOSServices::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
}

AOSServices::AOSServices(const AFilename& basePath) :
  mp_InputExecutor(NULL),
  mp_ModuleExecutor(NULL),
  mp_OutputExecutor(NULL),
  mp_Configuration(NULL),
  mp_DatabaseConnPool(NULL),
  mp_SessionManager(NULL),
  mp_ContextManager(NULL),
  mp_CacheManager(NULL),
  mp_Log(NULL)
{
  //a_Create log (must be done first as other objects rely on it)
  AFilename logfile(basePath);
  logfile.usePathNames().push_back(ASW("logs",4));
  logfile.useFilename().assign(ASW("aos.log",7));
  AFilename mutexfile(logfile);
  mutexfile.useFilename().assign(ASW("aos.mutex",9));
  mp_Log = new ALog_AFile(new ASync_Mutex(mutexfile.toAString()), logfile, ALog::ALL_ERRORS);

  //a_Must have admin registry before configuration
  mp_AdminRegistry = new AOSAdminRegistry(*mp_Log);

  //a_Read in the config file
  mp_Configuration = new AOSConfiguration(basePath, *this);

  //a_Now adjust log settings based on config
  AString str = mp_Configuration->useConfigRoot().getString("server/log/mask", AConstant::ASTRING_EMPTY);
  if (!str.isEmpty())
    mp_Log->setEventMask(str.toU4(16));
  u4 maxFileSize = mp_Configuration->useConfigRoot().getU4("server/log/max-file-size", ALog_AFile::DEFAULT_MAX_FILE_SIZE);
  mp_Log->setLoggerMaxFileSize(maxFileSize);

  mp_ContextManager = new AOSContextManager(*this);
  mp_CacheManager = new AOSCacheManager(*this);
  mp_InputExecutor = new AOSInputExecutor(*this);
  mp_ModuleExecutor = new AOSModuleExecutor(*this);
  mp_OutputExecutor = new AOSOutputExecutor(*this);
  mp_SessionManager = new AOSSessionManager(*this);
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
    delete mp_OutputExecutor;
    delete mp_ModuleExecutor;
    delete mp_InputExecutor;
    delete mp_Configuration;
    delete mp_Log;
  }
  catch(...) {}
}

bool AOSServices::initDatabasePool()
{
  AASSERT(this, mp_Configuration);
  mp_DatabaseConnPool = new AOSDatabaseConnectionPool(*this);

  int iMaxConnections = mp_Configuration->useConfigRoot().getInt(AOSConfiguration::DATABASE_CONNECTIONS, 2);
  AString strUrl = mp_Configuration->useConfigRoot().getString(AOSConfiguration::DATABASE_URL, AConstant::ASTRING_EMPTY);
  
  bool ret = true;
  
  //a_No URL, create a NOP server and skip global init
  if (strUrl.isEmpty())
    ret = false;

  AUrl urlServer(strUrl);
  mp_DatabaseConnPool->init(urlServer, iMaxConnections);
  
  return ret;
}

bool AOSServices::loadModules()
{
  AASSERT(this, mp_Configuration);
  AASSERT(this, mp_Log);
  AASSERT(this, mp_InputExecutor);
  AASSERT(this, mp_ModuleExecutor);
  AASSERT(this, mp_OutputExecutor);

  AFileSystem::FileInfos configList;
  mp_Configuration->getDynamicModuleConfigsToLoad(configList);
  for (AFileSystem::FileInfos::iterator it = configList.begin(); it != configList.end(); ++it)
  {
    //a_Load the corresponding XML config for each library if exists
    mp_Configuration->loadConfig((*it).filename);

    //a_Load dynamic library associated with it
    AString f;
    (*it).filename.emitFilenameNoExt(f);
    if (!m_Modules.load(f))
    {
      AString strDebug("Unable to load dynamic library: ");
      strDebug.append(f);
      mp_Log->add(strDebug, ALog::FAILURE);
      return false;
    }
    else
    {
      AString strDebug("=====[ BEGIN: Processing library '");
      strDebug.append(f);
      strDebug.append("' ]=====");
      AOS_DEBUGTRACE(strDebug.c_str(), NULL);
    }
    
    //a_Register input processors, modules, output generators
    PROC_AOS_Register *procRegister = static_cast<PROC_AOS_Register *>(m_Modules.getEntryPoint(f, "aos_register"));
    if (procRegister)
    {
      if (
        procRegister(
          *mp_InputExecutor, 
          *mp_ModuleExecutor, 
          *mp_OutputExecutor, 
          *this
        )
      )
      {
        AString strDebug("  FAILED to register module: '");
        strDebug.append(f);
        strDebug.append('\'');
        mp_Log->add(strDebug, ALog::FAILURE);
        return false;
      }
      else
      {
        AString strDebug("  Registered: '");
        strDebug.append(f);
        strDebug.append('\'');
        AOS_DEBUGTRACE(strDebug.c_str(), NULL);
      }
    }
    else
    {
      AString strDebug("  Module: '");
      strDebug.append(f);
      strDebug.append("': unable to find proc symbol: aos_register");
      mp_Log->add(strDebug, ALog::FAILURE);
      return false;
    }
  }
  
  return true;
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
        m_GlobalObjects.insert(resultSet.getData(u, nName), new AString(resultSet.getData(u, nValue)), true);
      }
    }
  }
  else
    return AConstant::npos;

  return resultSet.getRowCount();
}

ABasePtrContainer& AOSServices::useGlobalObjects()
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

ATemplate *AOSServices::createTemplate(u4 defaultLuaLibraries)
{
  AAutoPtr<ATemplate> pTemplate(new ATemplate(), true);
  
  //a_Add Lua handler
  pTemplate->addHandler(new ATemplateNodeHandler_LUA());

  //a_Add session data handler
  pTemplate->addHandler(new ATemplateNodeHandler_SESSION());

  pTemplate.setOwnership(false);
  return pTemplate;
}

AOSInputExecutor& AOSServices::useInputExecutor() 
{ 
  AASSERT(this, mp_InputExecutor);
  return *mp_InputExecutor;
}

AOSModuleExecutor& AOSServices::useModuleExecutor()
{ 
  AASSERT(this, mp_ModuleExecutor);
  return *mp_ModuleExecutor; 
}

AOSOutputExecutor& AOSServices::useOutputExecutor()
{ 
  AASSERT(this, mp_OutputExecutor);
  return *mp_OutputExecutor;
}

const ADynamicLibrary& AOSServices::getModules() const
{
  return m_Modules;
}
