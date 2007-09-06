#include "pchAOS_Base.hpp"
#include "AOSDatabaseConnectionPool.hpp"
#include "AMySQLServer.hpp"
#include "AODBCServer.hpp"
#include "ASQLiteServer.hpp"
#include "AOSServices.hpp"

AOSDatabaseConnectionPool::AOSDatabaseConnectionPool(AOSServices& services) :
  m_Services(services),
  mp_DatabasePool(NULL),
  m_isInitialized(false)
{
  registerAdminObject(m_Services.useAdminRegistry());
}

AOSDatabaseConnectionPool::~AOSDatabaseConnectionPool()
{
  try
  {
    pDelete(mp_DatabasePool);
  }
  catch(...) {}
}

const AString& AOSDatabaseConnectionPool::getClass() const
{
  static const AString CLASS("AOSDatabaseConnectionPool");
  return CLASS;
}

void AOSDatabaseConnectionPool::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{ 
  AOSAdminInterface::addAdminXml(eBase, request);

  addProperty(eBase, ASW("isInitialized",13), (m_isInitialized ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE));
  if (mp_DatabasePool)
    addProperty(eBase, ASW("DatabasePool",12), *mp_DatabasePool);
}

void AOSDatabaseConnectionPool::init(const AUrl& url, int count)
{
  AASSERT(this, count > 0);
  
  //a_Create template database
  ADatabase *pServer = NULL;
  switch (url.getProtocolEnum())
  {
    case AUrl::MYSQL:
      pServer = new AMySQLServer(url);
    break;

    case AUrl::ODBC:
      pServer = new AODBCServer(url);
    break;

    case AUrl::SQLITE:
      pServer = new ASQLiteServer(url);
    break;

    default:
      ATHROW(this, AException::InvalidProtocol);
  }  

  mp_DatabasePool = new ADatabasePool(pServer, count);
  m_isInitialized = true;
}

ADatabasePool& AOSDatabaseConnectionPool::useDatabasePool()
{ 
  AASSERT(this, mp_DatabasePool);
  return *mp_DatabasePool;
}

bool AOSDatabaseConnectionPool::isInitialized() const
{
  return m_isInitialized;
}
