/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSServices_HPP__
#define INCLUDED__AOSServices_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ALog_AFile.hpp"
#include "AOSAdminRegistry.hpp"
#include "AOSSessionManager.hpp"
#include "AOSContextManager.hpp"
#include "AOSCacheManager.hpp"
#include "AOSDatabaseConnectionPool.hpp"
#include "AOSConfiguration.hpp"
#include "ALuaEmbed.hpp"
#include "AOSInputExecutor.hpp"
#include "AOSModuleExecutor.hpp"
#include "AOSOutputExecutor.hpp"
#include "ADynamicLibrary.hpp"

class AOS_BASE_API AOSServices : public AOSAdminInterface
{
public:
  /*!
  Construct services from base aos_root path
  
  @param basePath of aos_root directory
  */
  AOSServices(const AFilename& basePath);
  
  //! virtual dtor
  virtual ~AOSServices();
  
  /*!
  The file log
  */
  ALog& useLog();
  
  /*!
  Combined XML configuration
  */
  AOSConfiguration& useConfiguration();
  
  /*!
  Database connection pool
  */
  AOSDatabaseConnectionPool& useDatabaseConnectionPool();
  
  /*!
  Admin object registry
  */
  AOSAdminRegistry& useAdminRegistry();

  /*!
  AOSInputExecutor
  */
  AOSInputExecutor& useInputExecutor();

  /*!
  AOSModuleExecutor
  */
  AOSModuleExecutor& useModuleExecutor();

  /*!
  AOSOutputExecutor
  */
  AOSOutputExecutor& useOutputExecutor();

  /*!
  Session manager
  */
  AOSSessionManager& useSessionManager();
  
  /*!
  AOSContext manager
  */
  AOSContextManager& useContextManager();

  /*!
  AOSContext manager
  */
  AOSCacheManager& useCacheManager();

  /*!
  Initialize database pool
  */
  bool initDatabasePool();

  /*!
  Initialize the global objects
  All data assumed in table 'global', columns 'name' and 'value'
  
  @param strError to write errors to
  @return number of rows loaded, AConstant::npos if error
  */
  size_t loadGlobalObjects(AString& strError);

  /*!
  Load modules specified in the ./aos_config/conf/load directory
  Every .xml file will load same named .dll/.so in dynamic library lookup path
  
  @return false if error occured
  */
  bool loadModules();

  /*!
  Global objects

  @return conatiners of global objects
  */
  ABasePtrContainer& useGlobalObjects();

  /*!
  Create an instance of ATemplate with CODE and LUA processor with AOS specific LUA library loaded 
  
  @see AOSLuaFunctions.hpp
  @see ALuaEmbed.hpp

  NOTE: OWNED and DELETED by caller
  */
  ATemplate* createTemplate(u4 deafultLuaLibraries = ALuaEmbed::LUALIB_ALL);

  /*!
  Get dynamic library object
  Used in loading modules and contains a map of module names to modules

  @return constant reference to dynamic library object
  */
  const ADynamicLibrary& getModules() const;

  /*!
  Admin xml
  */
  virtual void adminRegisterObject(AOSAdminRegistry& adminRegistry);
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_No default ctor
  AOSServices() {}

  //a_No copy ctor
  AOSServices(const AOSServices&) {}

  //a_No copy operator
  AOSServices& operator =(const AOSServices&) { return *this; }
  
  //a_Input executor
  AOSInputExecutor *mp_InputExecutor;

  //a_Module executor
  AOSModuleExecutor *mp_ModuleExecutor;

  //a_Input executor
  AOSOutputExecutor *mp_OutputExecutor;

  //a_Physical log
  ALog_AFile *mp_Log;
  
  //a_Global objects for all requests
  ABasePtrContainer m_GlobalObjects;

  //a_Configuration
  AOSConfiguration *mp_Configuration;
  
  //a_Admin registry
  AOSAdminRegistry *mp_AdminRegistry;

  //Database connection pool
  AOSDatabaseConnectionPool *mp_DatabaseConnPool;

  //a_Session manager
  AOSSessionManager *mp_SessionManager;

  //a_Context manager
  AOSContextManager *mp_ContextManager;

  //a_Cache manager
  AOSCacheManager *mp_CacheManager;

  //a_Modules to load
  ADynamicLibrary m_Modules;
};

#endif //INCLUDED__AOSServices_HPP__
