#ifndef INCLUDED__AOSServices_HPP__
#define INCLUDED__AOSServices_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ALog_AFile.hpp"
#include "AOSAdminRegistry.hpp"
#include "ASync_Mutex.hpp"
#include "AOSSessionManager.hpp"
#include "AOSContextManager.hpp"
#include "AOSCacheManager.hpp"
#include "AOSDatabaseConnectionPool.hpp"
#include "AOSConfiguration.hpp"

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

  /*!
  Used to synchronize console output using ALock class
  */
  ASynchronization& useScreenSynch() { return m_ConsoleSynch; }

private:
  //a_No default ctor
  AOSServices() : m_ConsoleSynch(AConstant::ASTRING_QUESTIONMARK) {}

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

  //a_Screen output synchronization
  ASync_Mutex m_ConsoleSynch;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSServices_HPP__
