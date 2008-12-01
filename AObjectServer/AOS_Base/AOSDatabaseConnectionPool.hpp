/*
Written by Alex Chachanashvili

$Id$
*/
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
  //! Class name
  static const AString CLASS;

public:
  /*!
  Create connection pool with 'count' connections
  
  \verbatim
  URL: mysql://<username>:<password>@[hostname]:<port>/[database]/   (MUST have trailing /)
  URL: odbc://<username>:<password>@[hostname]/[data source]/        (MUST have trailing /)
  URL: sqlite://driveletter:/path/databasefile
  \endverbatim
  
  @param services reference
  */
  AOSDatabaseConnectionPool(AOSServices& services);
  
  //! dtor
  virtual ~AOSDatabaseConnectionPool();
  
  /*!
  Initialize

  @param url to the database
  @param connections to open
  @throw AException if error occurs
  */
  void init(const AUrl& url, int connections);

  /*!
  Use the pool

  @return reference to the pool
  */
  ADatabasePool& useDatabasePool();

  /*!
  Checks if the pool was initialized

  @return true if initialized
  */
  bool isInitialized() const;

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  AOSServices& m_Services;

  ADatabasePool *mp_DatabasePool;
  
  bool m_isInitialized;
};

#endif //INCLUDED__AOSDatabaseConnectionPool_HPP__
