#ifndef INCLUDED__AOSDatabaseConnectionPool_HPP__
#define INCLUDED__AOSDatabaseConnectionPool_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AUrl.hpp"
#include "ADatabasePool.hpp"

class AOSServices;

class AOS_BASE_API AOSDatabaseConnectionPool : public AOSAdminInterface
{
public:
  /*!
  Create connection pool with 'count' connections
  URL: mysql://<username>:<password>@[hostname]:<port>/[database]/   (MUST have trailing /)
  URL: odbc://<username>:<password>@[hostname]/[data source]/        (MUST have trailing /)
  URL: sqlite://driveletter:/path/databasefile
  */
  AOSDatabaseConnectionPool(AOSServices&);
  virtual ~AOSDatabaseConnectionPool();
  
  /*!
  Initialize
  Will throw exception if an error occurs
  */
  void init(const AUrl& url, int count);

  /*!
  Use the pool
  */
  ADatabasePool& useDatabasePool();

  /*!
  Checks if the pool was initialized
  */
  bool isInitialized() const;

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  AOSServices& m_Services;

  ADatabasePool *mp_DatabasePool;
  
  bool m_isInitialized;
};

#endif //INCLUDED__AOSDatabaseConnectionPool_HPP__
