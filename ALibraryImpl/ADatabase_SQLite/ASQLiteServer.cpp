/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchADatabase_SQLite.hpp"
#include "ASQLiteServer.hpp"
#include "AResultSet.hpp"
#include "AXmlElement.hpp"
#include "AString.hpp"

void ASQLiteServer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ASQLiteServer @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADatabase::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent+1) << "mp_db=0x" << std::hex << (void *)mp_db << std::dec << std::endl;
  AString strPath;
  _getDBFilename(strPath);
  ADebugDumpable::indent(os, indent+1) << "DB filename=" << strPath << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ASQLiteServer::ASQLiteServer()
{
}

ASQLiteServer::ASQLiteServer(const AUrl& url) :
  ADatabase(url),
  mp_db(NULL)
{
}

ASQLiteServer::~ASQLiteServer()
{
  try
  {
    deinit();
  }
  catch(...) {}
}

bool ASQLiteServer::init(AString& error)
{
  AASSERT_EX(this, !mbool_Initialized, ASWNL("SQLite already initialized."));
  AASSERT_EX(this, !mp_db, ASWNL("sqlite3 object already exists."));

  AString strPath;
  _getDBFilename(strPath);
  int rc = sqlite3_open(strPath.c_str(), &mp_db);
  if(rc)
  {
    error.append("Can't open database: ");
    error.append(sqlite3_errmsg(mp_db));
    sqlite3_close(mp_db);
    return false;
  }

  mbool_Initialized=true;
  return true;
}

bool ASQLiteServer::reconnect(AString& error)
{
  //a_Not applicable to sqlite3
  ATHROW(this, AException::NotImplemented);
}

void ASQLiteServer::deinit()
{
  if (mbool_Initialized && mp_db)
  {
    sqlite3_close(mp_db);
  }
  
  mp_db = NULL;
  mbool_Initialized = false;
}

ADatabase *ASQLiteServer::clone(AString& error) const
{
  ASQLiteServer *pDatabase = new ASQLiteServer(m_urlConnection);
  if (!pDatabase->init(error))
  {
    delete pDatabase;
    return NULL;
  }
  return pDatabase;
}

void ASQLiteServer::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign("ASQLiteServer",11);

  ADatabase::emitXml(target);

  AString strPath;
  _getDBFilename(strPath);
  target.addElement(ASW("DBFilename",10), strPath, AXmlElement::ENC_CDATADIRECT);
}

sqlite3 *ASQLiteServer::getDBHandle()
{
  if (!isInitialized())
    ATHROW_EX(this, AException::InvalidObject, ASWNL("Database not initialized."));

  return mp_db;
}

size_t ASQLiteServer::executeSQL(const AString& query, AResultSet& target, AString& error)
{
  char *zErrMsg = NULL;

  target.clear();
  int rc = sqlite3_exec(mp_db, query.c_str(), ASQLiteServer::_callbackExecuteSQL, (void *)&target, &zErrMsg);
  if(SQLITE_OK != rc)
  {
    if (zErrMsg)
    {
      error.append(zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
      error.append("Error message: NULL");

    return AConstant::npos;
  }

  target.useSQL().assign(query);
  
  //a_Return either rows processed or total rows changed
  if (target.getRowCount() >= 0 && target.getFieldCount() > 0)
    return target.getRowCount();
  else
    return sqlite3_changes(mp_db);
}

int ASQLiteServer::_callbackExecuteSQL(void *pvParam, int fieldCount, char **pcData, char **pcColName)
{
  AResultSet *pResultSet = (AResultSet *)pvParam;
  AASSERT_EX(NULL, pResultSet, ASWNL("AResultSet is NULL"));

  //a_Get metadata if not yet saved
  if (pResultSet->useFieldNames().size() == 0)
  {
    pResultSet->useFieldNames().reserve(fieldCount);
    for(int i=0; i<fieldCount; i++)
      pResultSet->useFieldNames().push_back(pcColName[i]);
  }

  //a_Iterate over the columns and add data to the new row
  VECTOR_AString& row = pResultSet->insertRow();
  row.reserve(fieldCount);
  for(int i=0; i<fieldCount; i++)
  {
    row.push_back(pcData[i] ? pcData[i] : AConstant::ASTRING_NULL);
  }

  return 0;
}

void ASQLiteServer::_getDBFilename(AString& strPath) const
{
  strPath.assign(m_urlConnection.getServer());
  if (!strPath.isEmpty())
    strPath.append(':');
  strPath.append(m_urlConnection.getPathAndFilename());
}
