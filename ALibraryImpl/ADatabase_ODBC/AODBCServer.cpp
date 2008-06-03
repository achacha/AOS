/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchADatabase_ODBC.hpp"
#include "AODBCServer.hpp"
#include "AResultSet.hpp"
#include "AXmlElement.hpp"
#include "AString.hpp"

void AODBCServer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AODBCServer @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADatabase::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent+1) << "m_henv=" << m_henv << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_hdbc=" << m_hdbc << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AODBCServer::AODBCServer():
  m_henv(SQL_NULL_HENV),
  m_hdbc(SQL_NULL_HDBC)
{
}

AODBCServer::AODBCServer(const AUrl& url):
  ADatabase(url),
  m_henv(SQL_NULL_HENV),
  m_hdbc(SQL_NULL_HDBC)
{
}

AODBCServer::~AODBCServer()
{
  try
  {
    deinit();
  }
  catch(...) {}
}

AODBCServer::ODBC_METADATA_FIELD::ODBC_METADATA_FIELD() : 
  colName(256, 64),
  dataType(-1),
  colSize(-1),
  decimalDigits(-1),
  nullable(SQL_NULLABLE_UNKNOWN)
{
}

bool AODBCServer::init(AString& error)
{
  AASSERT_EX(this, !mbool_Initialized, ASWNL("ODBC data source already initialized."));

  SQLRETURN retcode;
  
  //a_Allocate environment handle
  retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
  if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
  {
    //a_Set the ODBC version environment attribute
    retcode = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
    {
      //a_Allocate connection handle
      retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc); 
      if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
      {
        //a_Set login timeout to 6 seconds
        int timeout = 6;
        SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER *)&timeout, SQL_IS_INTEGER);

         /* Connect to data source */
        retcode = SQLConnect(
          m_hdbc,
          (SQLCHAR*)m_urlConnection.getBaseDirName().c_str(), (SQLSMALLINT)m_urlConnection.getPath().getSize(),
          (SQLCHAR*)m_urlConnection.getUsername().c_str(), (SQLSMALLINT)m_urlConnection.getUsername().getSize(),
          (SQLCHAR*)m_urlConnection.getPassword().c_str(), (SQLSMALLINT)m_urlConnection.getPassword().getSize()
        );
        if (SQL_ERROR == retcode)
        {
          error.append("ODBC unable to connect:");
          error.append("\r\n  datasource=");
          error.append(m_urlConnection.getBaseDirName());
          error.append("\r\n");
          _processError(error, SQL_HANDLE_DBC, m_hdbc);
          SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
          SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
          return false;
        }
      }
      else
      {
        error.append("ODBC unable to allocate handle:");
        _processError(error, SQL_HANDLE_DBC, m_hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
        return false;
      }
    }
    else
    {
      error.append("ODBC unable to set version environment attribute:");
      _processError(error, SQL_HANDLE_ENV, m_henv);
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
      return false;
    }
  }
  else
  {
    error.append("ODBC unable to allocate environment:");
    _processError(error, SQL_HANDLE_ENV, m_henv);
    return false;
  }

  mbool_Initialized=true;
  return true;
}

bool AODBCServer::reconnect(AString& error)
{
  AASSERT_EX(this, !mbool_Initialized, ASWNL("ODBC data source already initialized."));

  if (NULL != m_hdbc)
    SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
  
  SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc); 
  if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
  {
    //a_Set login timeout to 6 seconds
    int timeout = 6;
    SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER *)&timeout, SQL_IS_INTEGER);

     /* Connect to data source */
    retcode = SQLConnect(
      m_hdbc,
      (SQLCHAR*)m_urlConnection.getBaseDirName().c_str(), (SQLSMALLINT)m_urlConnection.getPath().getSize(),
      (SQLCHAR*)m_urlConnection.getUsername().c_str(), (SQLSMALLINT)m_urlConnection.getUsername().getSize(),
      (SQLCHAR*)m_urlConnection.getPassword().c_str(), (SQLSMALLINT)m_urlConnection.getPassword().getSize()
    );
    if (SQL_ERROR == retcode)
    {
      error.append("ODBC unable to re-connect:");
      error.append("\r\n  datasource=");
      error.append(m_urlConnection.getBaseDirName());
      error.append("\r\n");
      _processError(error, SQL_HANDLE_DBC, m_hdbc);
      SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
      return false;
    }
  }
  else
  {
    error.append("ODBC unable to re-allocate handle:");
    _processError(error, SQL_HANDLE_DBC, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
    return false;
  }

  return true;
}

void AODBCServer::_processError(AString& error, SQLSMALLINT handleType, HANDLE handle)
{
  SQLCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH+1];
  SQLINTEGER    NativeError;
  SQLSMALLINT   i, MsgLen;

  i = 1;
  while(SQL_SUCCESS == SQLGetDiagRec(
      handleType,             //  HandleType,  (SQL_HANDLE_ENV, SQL_HANDLE_DBC, or SQL_HANDLE_STMT)
      handle,                 //  Handle, 
      i,                      //  RecNumber, 
      SqlState,               //  szSqlstate, 
      &NativeError,           //  pfNativeErrorPtr, 
      Msg,                    //  szErrorMsg, 
      SQL_MAX_MESSAGE_LENGTH, //  cbErrorMsgMax, 
      &MsgLen                 //  pcbErrorMsg
    )
  )  
  {
    error.append("ODBC_error[");
    error.append(AString::fromInt(i));
    error.append("]{\r\n\tSQLSTATE=");
    error.append((char *)SqlState);
    error.append("\r\n\tNativeError=");
    error.append(AString::fromInt(NativeError));
    error.append("\r\n\tMessage=");
    error.append((char *)Msg);
    error.append("\r\n}\r\n");
    ++i;
  }
}

void AODBCServer::deinit()
{
  if (mbool_Initialized)
  {
    SQLDisconnect(m_hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
  }

  mbool_Initialized = false;
}

ADatabase *AODBCServer::clone(AString& error) const
{
  AODBCServer *pDatabase = new AODBCServer(m_urlConnection);
  if (!pDatabase->init(error))
  {
    delete pDatabase;
    return NULL;
  }
  return pDatabase;
}

void AODBCServer::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("AODBCServer",11);

  ADatabase::emitXml(target);
}

bool AODBCServer::getTables(VECTOR_AString& sv, AString& error)
{
  if (!isInitialized())
  {
    error.assign("Database has not been initialized;");
    return false;
  }

  sv.clear();
  
//TODO:

  return true;
}

bool AODBCServer::getFields(const AString& table, VECTOR_AString& sv, AString& error)
{
  if (!isInitialized())
  {
    error.assign("Database has not been initialized;");
    return false;
  }

  if (table.isEmpty())
  {
    error = "Please use a namespace;";
    return false;
  }

  sv.clear();

//TODO:

  return true;
}

SQLHSTMT AODBCServer::executeSQL(const AString& query, AString& error)
{
  if (!isInitialized())
  {
    error.assign("Database not intialized;");
    return SQL_NULL_HSTMT;
  }

  SQLHSTMT hstmt(SQL_NULL_HSTMT);

  // Allocate statement handle
  int tries = 0;
  SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt); 
  while (tries < 3 && retcode == SQL_INVALID_HANDLE)
  {
    if (reconnect(error))
    {
      //a_Try again
      error.clear();
      retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt);
      continue;
    }
    ++tries;
  }
  if (3 == tries)
  {
    error.append("Unable to re-establish a connection after 3 tries.");
    return false;
  }

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
    _processError(error, SQL_HANDLE_STMT, hstmt);
    return SQL_NULL_HSTMT;
  }
  
  // Execute SQL
  retcode = SQLExecDirect(hstmt, (SQLCHAR *)query.c_str(), query.getSize());
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
    _processError(error, SQL_HANDLE_STMT, hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return SQL_NULL_HSTMT;
  }
  
  return hstmt;
}

size_t AODBCServer::executeSQL(const AString& query, AResultSet& target, AString& error)
{
  target.clear();
  SQLHSTMT hstmt = executeSQL(query, error);
  if (SQL_NULL_HSTMT == hstmt)
    return AConstant::npos;

  target.useSQL().assign(query);
  return _processQueryAllRows(hstmt, target, error);
}

size_t AODBCServer::_processQueryAllRows(SQLHSTMT hstmt, AResultSet& target, AString& error)
{
  //a_Get row count
  size_t rowCount = 0;
#ifdef __WINDOWS__
  #ifdef WIN64
    SQLRETURN retcode = SQLRowCount(hstmt, (SQLLEN *)&rowCount);
  #else
    SQLRETURN retcode = SQLRowCount(hstmt, (SQLINTEGER *)&rowCount);
  #endif
#endif
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
    _processError(error, SQL_HANDLE_STMT, hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return AConstant::npos;
  }
  
  //a_Get column metadata
  SQLUSMALLINT col;
  AODBCServer::METADATA metadata;
  SQLSMALLINT colCount = getMetadata(hstmt, metadata, error);
  if (colCount < 0)
  {
    _processError(error, SQL_HANDLE_STMT, hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return AConstant::npos;
  }

  //a_Populate column metadata in the result set
  for (col=0; col<colCount; ++col)
    target.useFieldNames().push_back(metadata[col].colName);

  //a_Fetch next row
  retcode = SQLFetch(hstmt); 
  if (SQL_ERROR == retcode)
  {
    return rowCount;  //a_No data, return rows affected
  }

  //a_Iterate over rows and process
  u4 row = 0;
  AString str(4096, 1024);
  SQLINTEGER dataLenOrNull;
  while(
    row < rowCount 
    && (
      SQL_NO_DATA != retcode 
      || SQL_SUCCESS == retcode 
      || SQL_SUCCESS_WITH_INFO == retcode
    )
  )
  {
    VECTOR_AString& rowVector = target.insertRow();
    rowVector.reserve(colCount);
    for (col=0; col<colCount; ++col)
    {
#ifdef __WINDOWS__
  #ifdef WIN64
      retcode = ::SQLGetData(
        hstmt, 
        col+1, 
        SQL_C_CHAR, 
        str.startUsingCharPtr(4096), 
        4096, 
        (SQLLEN *)&dataLenOrNull
      );
  #else
      retcode = ::SQLGetData(
        hstmt, 
        col+1, 
        SQL_C_CHAR, 
        str.startUsingCharPtr(4096), 
        4096, 
        &dataLenOrNull
      );
  #endif
#endif
      if (SQL_NO_TOTAL == retcode || SQL_NULL_DATA == retcode)
        str.stopUsingCharPtr(0);
      else
        str.stopUsingCharPtr(dataLenOrNull);

      rowVector.push_back(str);
    }

    //a_Next row
    retcode = SQLFetch(hstmt);
    ++row;
  }

  if (row < rowCount && retcode != SQL_SUCCESS)
  {
    _processError(error, SQL_HANDLE_STMT, hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return AConstant::npos;
  }

  SQLFreeHandle(SQL_HANDLE_STMT, hstmt);      //a_Release statement
  return rowCount;
}

SQLSMALLINT AODBCServer::getMetadata(SQLHSTMT hstmt, AODBCServer::METADATA& metadata, AString& error)
{
  if (!isInitialized())
  {
    error.assign("Database not intialized;");
    return -1;
  }
  if (SQL_NULL_HSTMT == hstmt)
  {
    error.assign("SQL statement is NULL (failed execute?);");
    return -1;
  }

  //a_Get column count
  SQLSMALLINT colCount;
  SQLRETURN retcode = SQLNumResultCols(hstmt, (SQLSMALLINT*) &colCount);
  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
  {
    return -1;
  }

  metadata.clear();
  SQLSMALLINT colActualSize;
  for (SQLUSMALLINT col=0; col<colCount; ++col)
  {
    metadata.push_back(ODBC_METADATA_FIELD());
    ODBC_METADATA_FIELD& field = metadata.back();
#ifdef __WINDOWS__
  #ifdef WIN64
    retcode = ::SQLDescribeCol(
      hstmt, 
      col+1, 
      (SQLCHAR *)field.colName.startUsingCharPtr(256), 255, &colActualSize,
      &field.dataType,
      (SQLULEN *)&field.colSize,
      &field.decimalDigits,
      &field.nullable
    );
  #else
    retcode = ::SQLDescribeCol(
      hstmt, 
      col+1, 
      (SQLCHAR *)field.colName.startUsingCharPtr(256), 255, &colActualSize,
      &field.dataType,
      &field.colSize,
      &field.decimalDigits,
      &field.nullable
    );
  #endif
#endif
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
    {
      _processError(error, SQL_HANDLE_STMT, hstmt);
      SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
      metadata.clear();
      return -1;
    }
    field.colName.stopUsingCharPtr(colActualSize);
  }

  return colCount;
}
