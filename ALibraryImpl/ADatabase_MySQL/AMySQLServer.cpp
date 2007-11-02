#include "pchADatabase_MySQL.hpp"
#include "AMySQLServer.hpp"
#include "AResultSet.hpp"
#include "AXmlElement.hpp"

#ifdef __DEBUG_DUMP__
void AMySQLServer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AMySQLServer @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_urlConnection=" << std::endl;
  m_urlConnection.debugDump(os,indent+2);
  
  ADebugDumpable::indent(os, indent+1) << "client: " << mysql_get_client_info() << std::endl;
  
  if (mbool_Initialized && mp_mydata)
  {
    ADebugDumpable::indent(os, indent+1) << mysql_stat(mp_mydata) << std::endl;
    ADebugDumpable::indent(os, indent+1) << "server: " << mysql_get_server_info(mp_mydata) << std::endl;
    ADebugDumpable::indent(os, indent+1) << "host  : " << mysql_get_host_info(mp_mydata) << std::endl;
    ADebugDumpable::indent(os, indent+1) << "mp_mydata=0x" << std::hex << (void*)mp_mydata << std::dec << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_mydata=NULL" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AMySQLServer::AMySQLServer() :
  mp_mydata(NULL)
{
}

AMySQLServer::AMySQLServer(const AUrl& url):
  ADatabase(url),
  mp_mydata(NULL)
{
}

AMySQLServer::~AMySQLServer()
{
  try
  {
    deinit();
  }
  catch(...) {}
}

bool AMySQLServer::init(AString& error)
{
  AASSERT_EX(this, !mp_mydata, ASWNL("mp_mydata is not NULL, init must have been already called"));
  mp_mydata = mysql_init((MYSQL*) 0);
  if (!mp_mydata)
  {
    error = "Unable to initialize MySQL library in call to initFactory();";
    return false;
  }
  
  if (!mysql_real_connect( 
    mp_mydata,
    m_urlConnection.getServer().c_str(),
    m_urlConnection.getUsername().c_str(),
    m_urlConnection.getPassword().c_str(),
    m_urlConnection.getBaseDirName().c_str(),
    m_urlConnection.getPort(),
    NULL,
    0 )
  )
  {
    error = "Unable to connect to MySQL server: ";
    m_urlConnection.emit(error);
    error += ";";
    return false;
  }
  
  mbool_Initialized = true;
  return true;
}

bool AMySQLServer::isInitialized() const
{
  return (mbool_Initialized && mp_mydata);
}

void AMySQLServer::deinit()
{
  if (mp_mydata)
  {
    mysql_close(mp_mydata);
    mp_mydata = NULL;
  }
  mbool_Initialized = false;
}

ADatabase *AMySQLServer::clone(AString& error) const
{
  AMySQLServer *pDatabase = new AMySQLServer(m_urlConnection);
  if (!pDatabase->init(error))
  {
    delete pDatabase;
    return NULL;
  }
  return pDatabase;
}

void AMySQLServer::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("AMySQLServer", 12);

  ADatabase::emitXml(target);

  target.addElement(ASW("client",6)).addData(mysql_get_client_info());
  if (mp_mydata)
  {
    target.addElement(ASW("stat",4)).addData(mysql_stat(mp_mydata)); 
    target.addElement(ASW("server",6)).addData(mysql_get_server_info(mp_mydata)); 
    target.addElement(ASW("host",4)).addData(mysql_get_host_info(mp_mydata)); 
  }
}

MYSQL_RES *AMySQLServer::executeSQL(const AString& query, AString& error)
{
  if (!isInitialized())
  {
    error.assign("Database not intialized;");
    return NULL;
  }

  if (query.isEmpty())
  {
    AASSERT(this, 0);
    error = "Query is empty;";
    return NULL;
  }

  if (mysql_real_query(mp_mydata, query.c_str(), query.getSize()))
  {    
    u4 u4Errno = mysql_errno(mp_mydata);
    error = "Error(";
    error += mysql_error(mp_mydata);
    error += ":errno=";
    error += AString::fromU4(u4Errno);
    error += ") for query(";
    error += query;
    error += ");";
    return NULL;
  }

  return mysql_store_result(mp_mydata);
}

bool AMySQLServer::getTables(VECTOR_AString& sv, AString& error)
{
  if (!isInitialized())
  {
    error.assign("Database has not been initialized;");
    return false;
  }

  sv.clear();
  
  MYSQL_RES *pmyresult = mysql_list_tables(mp_mydata, NULL);
  if (pmyresult)
  {
    MYSQL_ROW myrow;
    int iSize = (int)mysql_num_rows(pmyresult);
    for (int i=0; i < iSize; ++i)
    {
      myrow = mysql_fetch_row(pmyresult);
      if (myrow)
      {
        sv.push_back(myrow[0]);
      }
    }

    mysql_free_result(pmyresult);
  }
  else
  {
    error = "Error(";
    error += mysql_error(mp_mydata);
    error += ") looking for tables;";
    return false;
  }

  return true;
}

bool AMySQLServer::getFields(const AString& table, VECTOR_AString& sv, AString& error)
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

  AString query("SHOW COLUMNS FROM `");
  query += table;
  query += "`";
  
  MYSQL_RES *pmyresult = executeSQL(query, error);
  if (pmyresult)
  {
    MYSQL_ROW myrow;
    int iSize = (int)mysql_num_rows(pmyresult);
    for (int i=0; i < iSize; ++i)
    {
      myrow = mysql_fetch_row(pmyresult);
      if (myrow)
      {
        sv.push_back(myrow[0]);
      }
    }

    mysql_free_result(pmyresult);
  }
  else
    return false;

  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, AString& result)
{
  const char *p = row[index];
  result.clear();
  if (p)
    result = p;

  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, AQueryString& result)
{
  const char *p = row[index];
  result.clear();
  if (p)
    result.parse(AString(p));
  
  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, ANumber& result)
{
  const char *p = row[index];
  result.clear();
  if (p)
    result.setNumber(p);
  
  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, ABitArray& result)
{
  const char *p = row[index];
  result.all();
  if (p)
    result.set(p);

  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, int& result)
{
  const char *p = row[index];
  result = 0;
  if (p)
    result = AString(p).toInt();
  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, s4& result)
{
  const char *p = row[index];
  result = 0;
  if (p)
    result = AString(row[index]).toS4();

  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, u4& result)
{
  const char *p = row[index];
  result = 0x0;
  if (p)
    result = AString(row[index]).toU4();

  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, s2& result)
{
  const char *p = row[index];
  result = 0;
  if (p)
    result = (s2)AString(row[index]).toS4();

  return true;
}

bool AMySQLServer::convert(MYSQL_ROW row, int index, u2& result)
{
  const char *p = row[index];
  result = 0x0;
  if (p)
    result = (u2)AString(row[index]).toU4();

  return true;
}

size_t AMySQLServer::executeSQL(const AString& sqlQuery, AResultSet& target, AString& error)
{
  target.clear();
  MYSQL_RES *pmyresult = executeSQL(sqlQuery, error);
  if (!pmyresult && !error.isEmpty())
    return AConstant::npos;

  target.useSQL().assign(sqlQuery);
  
  return (pmyresult ? _processQueryAllRows(pmyresult, target) : 0);
}

u4 AMySQLServer::_processQueryAllRows(MYSQL_RES *pmyresult, AResultSet& target)
{
  u4 rowCount = 0;
  if (pmyresult)
  {
    MYSQL_ROW myrow;
    rowCount = (int)mysql_num_rows(pmyresult);

    if (rowCount == 0)
      return 0;

    //a_Get field info
    MYSQL_FIELD *fields = mysql_fetch_fields(pmyresult);
    u4 num_fields = mysql_num_fields(pmyresult);

    //a_Set up field data
    for (u4 u=0; u<num_fields; ++u)
      target.useFieldNames().push_back(fields[u].name);

    //a_Populate results
    AString str;
    myrow = mysql_fetch_row(pmyresult);
    while (myrow)
    {
      VECTOR_AString& row = target.insertRow();
      row.reserve(num_fields);
      for(u4 field=0; field < num_fields; ++field)
      {
        row.push_back(myrow[field]);
      }
      
      myrow = mysql_fetch_row(pmyresult);
    }
  }

  mysql_free_result(pmyresult);
  return rowCount;
}
