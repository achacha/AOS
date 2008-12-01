/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_ExecuteQuery.hpp"

const AString AOSModule_ExecuteQuery::CLASS("ExecuteQuery");

const AString& AOSModule_ExecuteQuery::getClass() const
{
  return CLASS;
}

AOSModule_ExecuteQuery::AOSModule_ExecuteQuery(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_ExecuteQuery::execute(AOSContext& context, const AXmlElement& params)
{
  AString sql, path;
  params.emitString(AOS_BaseModules_Constants::SQL, sql);
  params.emitString(AOS_BaseModules_Constants::PATH, path);
  
  if (path.isEmpty())
    path.assign(ASW("ExecuteQuery/query", 18));

  if (!sql.isEmpty())
  {
    AString strError;
    AResultSet resultSet;
    if (AConstant::npos != context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(sql, resultSet, strError))
    {
      resultSet.emitXml(context.useModel().addElement(path));
    }
    else
    {
      context.addError(ASWNL("AOSModule_ExecuteQuery::execute"), strError);
      return AOSContext::RETURN_ERROR;
    }
  }
  else
  {
    context.addError(ASWNL("AOSModule_ExecuteQuery::execute"), ASWNL("/module/sql not found"));
    context.useModel().addElement("error").addElement(getClass()).addElement("params").addContent(params);
  }

  return AOSContext::RETURN_OK;
}

