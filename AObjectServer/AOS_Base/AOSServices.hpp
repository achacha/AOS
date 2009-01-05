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
#include "AOSResourceManager.hpp"

class AOS_BASE_API AOSServices : public AOSAdminInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  /*!
  Construct services from base aos_root path
  
  @param basePath of aos_root directory
  */
  AOSServices(const AFilename& basePath);
  
  //! virtual dtor
  virtual ~AOSServices();
  
  /*!
  Initialize services
  MUST be called after constructor when ready to create services
  */
  void init();

  /*!
  Get base path of the configuration to use
  
  @return constant reference to AFilename object
  */
  const AFilename& getBaseBath() const;
  
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
  AOSContext manager
  */
  AOSResourceManager& useResourceManager();

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
  // No default ctor
  AOSServices() {}

  // No copy ctor
  AOSServices(const AOSServices&) {}

  // No copy operator
  AOSServices& operator =(const AOSServices&) { return *this; }
  
  // Base path
  AFilename m_BasePath;
  
  // Input executor
  AOSInputExecutor *mp_InputExecutor;

  // Module executor
  AOSModuleExecutor *mp_ModuleExecutor;

  // Input executor
  AOSOutputExecutor *mp_OutputExecutor;

  // Physical log
  ALog_AFile *mp_Log;
  
  // Global objects for all requests
  ABasePtrContainer m_GlobalObjects;

  // Configuration
  AOSConfiguration *mp_Configuration;
  
  // Admin registry
  AOSAdminRegistry *mp_AdminRegistry;

  //Database connection pool
  AOSDatabaseConnectionPool *mp_DatabaseConnPool;

  // Session manager
  AOSSessionManager *mp_SessionManager;

  // Context manager
  AOSContextManager *mp_ContextManager;

  // Cache manager
  AOSCacheManager *mp_CacheManager;

  // Resource manager
  AOSResourceManager *mp_ResourceManager;
  
  // Modules to load
  ADynamicLibrary m_Modules;
};

#endif //INCLUDED__AOSServices_HPP__
