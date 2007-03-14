#ifndef INCLUDED__AOSServices_HPP__
#define INCLUDED__AOSServices_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ALog_AFile.hpp"
#include "AOSAdminRegistry.hpp"
#include "AMutex.hpp"
#include "AOSSessionManager.hpp"
#include "AOSContextManager.hpp"
#include "AOSDatabaseConnectionPool.hpp"

class AFilename;
class AOSConfiguration;

class AOS_BASE_API AOSServices : public AOSAdminInterface
{
public:
  AOSServices(const AFilename& iniFilename);
  AOSServices(const AFilename& iniFilename, ALog::EVENT_MASK mask);     //a_Optionally set the log mask
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
  //a_Physical log
  ALog_AFile m_Log;
  
  //a_Global objects for all requests
  AObjectContainer m_GlobalObjects;

  //a_Configuration
  AOSConfiguration *mp_Configuration;
  
  //a_Admin registry
  AOSAdminRegistry m_AdminRegistry;

  //Database connection pool
  AOSDatabaseConnectionPool *mp_DatabaseConnPool;

  //a_Session manager
  AOSSessionManager *mp_SessionManager;

  //a_Session manager
  AOSContextManager *mp_ContextManager;

  //a_Screen output synchronization
  AMutex m_ConsoleSynch;

  //a_Initialize database pool
  void _initDatabasePool();

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSServices_HPP__
