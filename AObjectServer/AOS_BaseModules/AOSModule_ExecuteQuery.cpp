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

bool AOSModule_ExecuteQuery::execute(AOSContext& context, const AXmlElement& params)
{
#pragma message("AOSModule_ExecuteQuery::execute: needs work")
  AString sql, path;
  params.emitFromPath(ASW("sql", 3), sql);
  params.emitFromPath(ASW("path", 4), path);
  
  if (path.isEmpty())
    path.assign(ASW("ExecuteQuery/query", 18));

  if (!sql.isEmpty())
  {
    AString strError;
    AResultSet resultSet;
    if (AConstant::npos != context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(sql, resultSet, strError))
    {
      context.useOutputRootXmlElement().addElement(path).addData(resultSet, AXmlElement::ENC_CDATADIRECT);
    }
    else
    {
      context.addError("AOSModule_ExecuteQuery::execute", strError);
    }
  }
  else
  {
    context.addError("AOSModule_ExecuteQuery::execute", "/module/sql not found");
    context.useOutputRootXmlElement().addElement("error").addElement(getClass()).addElement("params").addContent(params);
  }

  return true;
}

