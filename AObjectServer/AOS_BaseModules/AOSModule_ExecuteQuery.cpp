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

AOSModule_ExecuteQuery::AOSModule_ExecuteQuery(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_ExecuteQuery::execute(AOSContext& context, const AXmlElement& params)
{
#pragma message("AOSModule_ExecuteQuery::execute: needs work")
  AString sql, path;
  params.emitFromPath(ASW("/params/module/sql", 18), sql);
  params.emitFromPath(ASW("/params/module/path", 19), path);
  
  if (path.isEmpty())
    path.assign(ASW("/ExecuteQuery/query", 19));

  if (!sql.isEmpty())
  {
    AString strError;
    AResultSet resultSet;
    if (AConstant::npos != context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(sql, resultSet, strError))
    {
      context.useOutputRootXmlElement().addElement(path, resultSet);
    }
    else
    {
      context.addError("AOSModule_ExecuteQuery::execute", strError);
    }
  }
  else
  {
    context.addError("AOSModule_ExecuteQuery::execute", "/params/sql not found");
    context.useOutputRootXmlElement().addElement("error").addElement(getClass()).addElement("params").addContent(params);
  }

  return true;
}

