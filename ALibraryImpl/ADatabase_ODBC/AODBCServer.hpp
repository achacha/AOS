#ifndef INCLUDED__AODBCServer_HPP__
#define INCLUDED__AODBCServer_HPP__

#include "apiADatabase_ODBC.hpp"
#include "ADatabase.hpp"

/*!
ODBC access class
NOTE: Design of the class is for multithreaded access
      This is why the string error gets passes to every call,
        check return if false, error has the error message

odbc://<username>:<password>@[server]/[data source name]/
*/
class ADATABASE_ODBC_API AODBCServer : public ADatabase
{
public:
  AODBCServer();
  AODBCServer(const AUrl& url);
  virtual ~AODBCServer();
  
  /*!
  ADatabase
  */
  virtual bool init(AString& error);
  virtual void deinit();
  virtual bool reconnect(AString& error);
  virtual ADatabase *clone(AString& error) const;
  
  /*!
  AObjectDatabase methods
  Returns # of rows
  Populates AResultSet
  */
  virtual size_t executeSQL(const AString& query, AResultSet&, AString& error);

  /*!
  Native ODBC return
  If invalid SQL_NULL_HSTMT is returned
  NOTE: You MUST release the handle in your code using SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
  */
  SQLHSTMT executeSQL(const AString& query, AString& error);

  /*!
  Utility functions for table
   Usage sample:
     AString error;          //Error in here if call returns false
     VECTOR_AString& tables;    //Storage vector for the table names
     if (db.getTables(tables, error))
       std::cout << error.c_str() << std::endl;
     ...
  */
  bool getTables(VECTOR_AString& sv, AString& error);
  bool getFields(const AString& table, VECTOR_AString& sv, AString& error);

  /*!
  AXmlEmittable interface
  */
  virtual void emit(AXmlElement&) const;

  /*!
  Metadata column
  */
  class ODBC_METADATA_FIELD
  {
  public:
    /*!
    Name of the column
    */
    AString colName;
    
    /*!
    See end of this file for ODBC SQL types
    */
    SQLSMALLINT dataType;           //a_ODBC data type

    /*!
    Size of the column on the data source.
    If the column size cannot be determined, the driver returns 0.
    */
    SQLUINTEGER colSize;            //a_Size of the column

    /*!
    Number of decimal digits of the column on the data source.
    If the number of decimal digits cannot be determined or is not applicable, the driver returns 0
    */
    SQLSMALLINT decimalDigits;      //a_Digits the decimal field (if numeric)

    /*!
    SQL_NO_NULLS: The column does not allow NULL values. 
    SQL_NULLABLE: The column allows NULL values. 
    SQL_NULLABLE_UNKNOWN: The driver cannot determine if the column allows NULL values. 
    */
    SQLSMALLINT nullable;

    //ctor
    ODBC_METADATA_FIELD();
  };

  typedef std::vector<ODBC_METADATA_FIELD> METADATA;

  /*!
  Metadata of the given statement result
  Returns number of fields in metadata, -1 if error
  */
  SQLSMALLINT getMetadata(SQLHSTMT hstmt, AODBCServer::METADATA& metadata, AString& error);


private:
  //a_ODBC specific
  SQLHENV m_henv;        // ODBC environment
  SQLHDBC m_hdbc;        // ODBC connection handle

  //a_Error processing
  void _processError(AString& error, SQLSMALLINT handleType, HANDLE handle);

  //a_Query processing
  size_t _processQueryAllRows(SQLHSTMT hstmt, AResultSet& target, AString& error);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AODBCServer_HPP__

/*
SQL type identifier[1]        Typical SQL data type[2] Typical type description 
SQL_CHAR                      CHAR(n) Character string of fixed string length n. 
SQL_VARCHAR                   VARCHAR(n) Variable-length character string with a maximum string length n. 
SQL_LONGVARCHAR               LONG VARCHAR Variable length character data. Maximum length is data source?dependent.[9] 
SQL_WCHAR                     WCHAR(n) Unicode character string of fixed string length n 
SQL_WVARCHAR                  VARWCHAR(n) Unicode variable-length character string with a maximum string length n 
SQL_WLONGVARCHAR              LONGWVARCHAR Unicode variable-length character data. Maximum length is data source?dependent 
SQL_DECIMAL                   DECIMAL(p,s) Signed, exact, numeric value with a precision of at least p and scale s. (The maximum precision is driver-defined.) (1 <= p <= 15; s <= p).[4] 
SQL_NUMERIC                   NUMERIC(p,s) Signed, exact, numeric value with a precision p and scale s (1 <= p <= 15; s <= p).[4] 
SQL_SMALLINT                  SMALLINT Exact numeric value with precision 5 and scale 0 (signed: -32,768 <= n <= 32,767, unsigned: 0 <= n <= 65,535)[3]. 
SQL_INTEGER                   INTEGER Exact numeric value with precision 10 and scale 0 (signed: ?2[31] <= n <= 2[31] ? 1, unsigned: 0 <= n <= 2[32] ? 1)[3]. 
SQL_REAL                      REAL Signed, approximate, numeric value with a binary precision 24 (zero or absolute value 10[?38] to 10[38]). 
SQL_FLOAT                     FLOAT(p) Signed, approximate, numeric value with a binary precision of at least p. (The maximum precision is driver-defined.)[5] 
SQL_DOUBLE                    DOUBLE PRECISION Signed, approximate, numeric value with a binary precision 53 (zero or absolute value 10[?308] to 10[308]). 
SQL_BIT                       BIT Single bit binary data.[8] 
SQL_TINYINT                   TINYINT Exact numeric value with precision 3 and scale 0 (signed: ?128 <= n <= 127, unsigned: 0 <= n <= 255)[3]. 
SQL_BIGINT                    BIGINT Exact numeric value with precision 19 (if signed) or 20 (if unsigned) and scale 0 (signed: ?2[63] <= n <= 2[63] ? 1, unsigned: 0 <= n <= 2[64] ? 1)[3],[9].
SQL_BINARY                    BINARY(n) Binary data of fixed length n.[9] 
SQL_VARBINARY                 VARBINARY(n) Variable length binary data of maximum length n. The maximum is set by the user.[9] 
SQL_LONGVARBINARY             LONG VARBINARY Variable length binary data. Maximum length is data source?dependent.[9] 
SQL_TYPE_DATE[6]              DATE Year, month, and day fields, conforming to the rules of the Gregorian calendar. (See "Constraints of the Gregorian Calendar," later in this appendix.) 
SQL_TYPE_TIME[6]              TIME(p) Hour, minute, and second fields, with valid values for hours of 00 to 23, valid values for minutes of 00 to 59, and valid values for seconds of 00 to 61. Precision p indicates the seconds precision. 
SQL_TYPE_TIMESTAMP[6]         TIMESTAMP(p) Year, month, day, hour, minute, and second fields, with valid values as defined for the DATE and TIME data types. 
SQL_TYPE_UTCDATETIME          UTCDATETIME Year, month, day, hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision. 
SQL_TYPE_UTCTIME              UTCTIME Hour, minute, second, utchour, and utcminute fields. The utchour and utcminute fields have 1/10th microsecond precision.. 
SQL_INTERVAL_MONTH[7]         INTERVAL MONTH(p) Number of months between two dates; p is the interval leading precision. 
SQL_INTERVAL_YEAR[7]          INTERVAL YEAR(p) Number of years between two dates; p is the interval leading precision. 
SQL_INTERVAL_YEAR_TO_MONTH[7] INTERVAL YEAR(p) TO MONTH Number of years and months between two dates; p is the interval leading precision. 
SQL_INTERVAL_DAY[7]           INTERVAL DAY(p) Number of days between two dates; p is the interval leading precision. 
SQL_INTERVAL_HOUR[7]          INTERVAL HOUR(p) Number of hours between two date/times; p is the interval leading precision. 
SQL_INTERVAL_MINUTE[7]        INTERVAL MINUTE(p) Number of minutes between two date/times; p is the interval leading precision. 
SQL_INTERVAL_SECOND[7]        INTERVAL SECOND(p,q) Number of seconds between two date/times; p is the interval leading precision and q is the interval seconds precision. 
SQL_INTERVAL_DAY_TO_HOUR[7]   INTERVAL DAY(p) TO HOUR Number of days/hours between two date/times; p is the interval leading precision. 
SQL_INTERVAL_DAY_TO_MINUTE[7] INTERVAL DAY(p) TO MINUTE Number of days/hours/minutes between two date/times; p is the interval leading precision. 
SQL_INTERVAL_DAY_TO_SECOND[7] INTERVAL DAY(p) TO SECOND(q) Number of days/hours/minutes/seconds between two date/times; p is the interval leading precision and q is the interval seconds precision. 
SQL_INTERVAL_HOUR_TO_MINUTE[7] INTERVAL HOUR(p) TO MINUTE Number of hours/minutes between two date/times; p is the interval leading precision. 
SQL_INTERVAL_HOUR_TO_SECOND[7] INTERVAL HOUR(p) TO SECOND(q) Number of hours/minutes/seconds between two date/times; p is the interval leading precision and q is the interval seconds precision. 
SQL_INTERVAL_MINUTE_TO_SECOND[7] INTERVAL MINUTE(p) TO SECOND(q) Number of minutes/seconds between two date/times; p is the interval leading precision and q is the interval seconds precision. 
SQL_GUID                      GUID Fixed length Globally Unique Identifier. 

[1]   This is the value returned in the DATA_TYPE column by a call to SQLGetTypeInfo.
[2]   This is the value returned in the NAME and CREATE PARAMS column by a call to SQLGetTypeInfo. The NAME column returns the designation?for example, CHAR?while the CREATE PARAMS column returns a comma-separated list of creation parameters such as precision, scale, and length.
[3]   An application uses SQLGetTypeInfo or SQLColAttribute to determine if a particular data type or a particular column in a result set is unsigned.
[4]   SQL_DECIMAL and SQL_NUMERIC data types differ only in their precision. The precision of a DECIMAL(p,s) is an implementation-defined decimal precision that is no less than p, while the precision of a NUMERIC(p,s) is exactly equal to p.
[5]   Depending on the implementation, the precision of SQL_FLOAT can be either 24 or 53: if it is 24, the SQL_FLOAT data type is the same as SQL_REAL; if it is 53, the SQL_FLOAT data type is the same as SQL_DOUBLE.
[6]   In ODBC 3.x, the SQL date, time, and timestamp data types are SQL_TYPE_DATE, SQL_TYPE_TIME, and SQL_TYPE_TIMESTAMP, respectively; in ODBC 2.x, the data types are SQL_DATE, SQL_TIME, and SQL_TIMESTAMP.
[7]   For more information on the interval SQL data types, see the Interval Data Types section, later in this appendix.
[8]   The SQL_BIT data type has different characteristics than the BIT type in SQL-92.
[9]   This data type has no corresponding data type in SQL-92.


*/

