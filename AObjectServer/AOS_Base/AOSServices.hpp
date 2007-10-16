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

class AFilename;

class AOS_BASE_API AOSServices : public AOSAdminInterface
{
public:
  AOSServices(const AFilename& basePath, ALog::EVENT_MASK mask = ALog::DEFAULT);
  virtual ~AOSServices();
  
  /*!
  Access to services
  */
  ALog& useLog();
  AOSConfiguration& useConfiguration();
  AOSDatabaseConnectionPool& useDatabaseConnectionPool();
  AOSAdminRegistry& useAdminRegistry();

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
  Create an instance of ATemplate with CODE and LUA processor with AOS specific LUA library loaded 
  
  @see AOSLuaFunctions.hpp
  @see ALuaEmbed.hpp

  NOTE: OWNED and DELETED by caller
  */
  ATemplate* createTemplate(u4 deafultLuaLibraries = ALuaEmbed::LUALIB_ALL);

  /*!
  Initialize database pool
  */
  bool initDatabasePool();

  /*!
  Initialize the global objects
  All data assumed in table 'global', columns 'name' and 'value'
  returns # of rows loaded, AConstant::npos if error
  */
  size_t loadGlobalObjects(AString& strError);

  /*!
  Global objects
  */
  AObjectContainer& useGlobalObjects();

  /*!
  Admin xml
  */
  virtual void registerAdminObject(AOSAdminRegistry& registry);
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //a_No default ctor
  AOSServices() {}

  //a_Physical log
  ALog_AFile *mp_Log;
  
  //a_Global objects for all requests
  AObjectContainer m_GlobalObjects;

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

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSServices_HPP__
