#ifndef INCLUDED__ADatabasePool_HPP__
#define INCLUDED__ADatabasePool_HPP__

#include "ADebugDumpable.hpp"
#include "ADatabase.hpp"
#include "ASync_CriticalSection.hpp"
#include "ASync_Semaphore.hpp"
#include "ATimer.hpp"
#include "AMovingAverage.hpp"

/*!
Ability to manage a homogenous pool of database connection under one facade
*/
class ABASE_API ADatabasePool : public ADebugDumpable
{
public:
  /*!
  ctor: pDatabase is a pointer to a valid ADatabase object type (can be initialized or not)
        maxConnections is how many total pDatabase objects to pool

  ADatabase::clone() is called maxConnections-1 times, pDatabase becomes one of the connections in the pool
  pDatabase is OWNED and DELETED by this object as are all the clones

  Usage:
    ADatabasePool myDBPool(new AMySQLServer(...), 10);
    myDBPool.executeSQL(...);
  */
  ADatabasePool(ADatabase *pDatabase, int maxConnections = 6);

  /*!
  dtor: deinits and deletes all ADatabase objects add'ed
  */
  virtual ~ADatabasePool();

  /*!
  AXmlElement
  */
  void emitXml(AXmlElement& target) const;

  /*!
  Add a database to the pool, this will be OWNED and DELETED by this class

  Usage:
    mypool.add(new MyDatabase());
    mypool.add(new MyOtherDatabase());
  */
  void add(ADatabase *);

  /*!
  Execute SQL against one of the free pool connections
  */
  size_t executeSQL(const AString& query, AResultSet&, AString& error);

private:
  class ADatabaseHolder
  {
  public:
    ADatabaseHolder(ADatabase *pDB) : mp_Database(pDB) {}
    ADatabase *mp_Database;
    ATimer m_inUseTimer;
    AMovingAverage m_averageTimeInUse;

  private:
    ADatabaseHolder() {}
  };
  
  typedef std::list<ADatabaseHolder *> DATABASES;
  DATABASES m_FreeDatabases;
  DATABASES m_LockedDatabases;
  
  ADatabase *_lockDatabase();
  void _unlockDatabase(ADatabase *);
  ASync_Semaphore m_Semaphore;
  ASync_CriticalSection m_SynchObject;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ADatabasePool_HPP__
