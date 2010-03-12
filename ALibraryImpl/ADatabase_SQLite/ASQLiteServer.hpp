/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ASQLiteServer_HPP__
#define INCLUDED__ASQLiteServer_HPP__

#include "apiADatabase_SQLite.hpp"
#include "ADatabase.hpp"

//a_From sqlite
typedef struct sqlite3 sqlite3;

/*!
SQLite access class ( http://www.sqlite.org/ )

NOTE: Design of the class is for multithreaded access
      This is why the string error gets passes to every call,
        check return if false, error has the error message

\verbatim
sqlite://[database file path]   

  e.g. sqlite://c:/tmp/mydatabase.db
\endverbatim
*/
class ADATABASE_SQLITE_API ASQLiteServer : public ADatabase
{
public:
  ASQLiteServer();
  
  /*!
  @param url to connect to e.g. sqlite://c:/tmp/mydatabase.db or sqlite:///./relative/path/my.db
  @param createNewIfNotFound if database does not exist, should a new one be created
  */
  ASQLiteServer(const AUrl& url, bool createNewIfNotFound = false);
  virtual ~ASQLiteServer();
  
  /*!
  ADatabase
  */
  virtual bool init(AString& error);
  virtual bool reconnect(AString& error);
  virtual void deinit();
  virtual ADatabase *clone(AString& error) const;
  
  /*!
  AObjectDatabase methods
  Returns # of rows
  Populates AResultSet
  */
  virtual size_t executeSQL(const AString& query, AResultSet&, AString& error);

  /*!
  AXmlEmittable interface
  */
  virtual void emit(AXmlElement&) const;

  /*!
  Internal DB connection object
  */
  sqlite3 *getDBHandle();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_SQLite specific
  sqlite3 *mp_db;

  //a_If new is to be created if file not found
  bool m_CreateNew;
  
  /*
  SELECT query processing
  pvParam is a pointer to AResultSet
  */
  static int _callbackExecuteSQL(void *pvParam, int colCount, char **pcData, char **pcColName);

  //a_Calculate effective filename from URL
  void _getDBFilename(AString&) const;
};

#endif //INCLUDED__ASQLiteServer_HPP__
