#include "pchAOS_BaseModules.hpp"
#include "AOSModule_ExecuteQuery.hpp"
#include "AOSCommand.hpp"
#include "AResultSet.hpp"
#include "ADatabase.hpp"

const AString& AOSModule_ExecuteQuery::getClass() const
{
  static const AString CLASS("ExecuteQuery");
  return CLASS;
}

AOSModule_ExecuteQuery::AOSModule_ExecuteQuery(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_ExecuteQuery::execute(AOSContext& context, const AXmlElement& params)
{
#pragma message("AOSModule_ExecuteQuery::execute: needs work")
  AString sql, path;
  params.emitString(ASW("sql", 3), sql);
  params.emitString(ASW("path", 4), path);
  
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

