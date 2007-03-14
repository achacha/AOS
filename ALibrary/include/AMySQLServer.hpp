#ifndef INCLUDED__AMySQLServer_HPP__
#define INCLUDED__AMySQLServer_HPP__

#include "apiADatabase_MySQL.hpp"
#include "ADatabase.hpp"

class ANumber;
class ABitArray;
class AQueryString;

/*!
MySQL access class
NOTE: Design of the class is for multithreaded access
      This is why the string strError gets passes to every call,
        check return if false, strError has the error message

mysql://<username>:<password>@[hostname]:<port>/[database]/        
*/
class ADATABASE_MYSQL_API AMySQLServer : public ADatabase
{
public:
  AMySQLServer();
  AMySQLServer(const AUrl& url);
  virtual ~AMySQLServer();
    
  /*!
  ADatabase basics
  */
  virtual bool init(AString& error);
  virtual bool isInitialized() const;
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
  MySQL specific
  NOTE: Return MYSQL_RES object that MUST be released with mysql_free_result() call
  */
  MYSQL_RES *executeSQL(const AString& query, AString& error);
  
  /*!
  Utility functions for table
   Usage sample:
     AString strError;          //Error in here if call returns false
     VECTOR_AString& tables;    //Storage vector for the table names
     if (db.getTables(tables, strError))
       std::cout << strError.c_str() << std::endl;
     ...
  */
  bool getTables(VECTOR_AString& sv, AString& error);
  bool getFields(const AString& table, VECTOR_AString& sv, AString& error);

  /*!
  Conversion utilities that work on the row
  */
  static bool convert(MYSQL_ROW row, int index, AString& result);
  static bool convert(MYSQL_ROW row, int index, AQueryString& result);
  static bool convert(MYSQL_ROW row, int index, ANumber& result);
  static bool convert(MYSQL_ROW row, int index, ABitArray& result);
  static bool convert(MYSQL_ROW row, int index, int& result);
  static bool convert(MYSQL_ROW row, int index, s4& result);
  static bool convert(MYSQL_ROW row, int index, u4& result);
  static bool convert(MYSQL_ROW row, int index, s2& result);
  static bool convert(MYSQL_ROW row, int index, u2& result);

private:
  MYSQL *mp_mydata;
  
  //a_Reads all rows and populates the AObjectContainer, returns rows processed
  u4 _processQueryAllRows(MYSQL_RES *pmyresult, AResultSet& resultSet);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AMySQLServer_HPP__

