#include "pchAOS_Wiki.hpp"
#include "AOSModule_WikiViewFromDatabase.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_WikiViewFromDatabase::getClass() const
{
  static const AString CLASS("WikiViewFromDatabase");
  return CLASS;
}

AOSModule_WikiViewFromDatabase::AOSModule_WikiViewFromDatabase(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_WikiViewFromDatabase::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AString strTable;
  if (!moduleParams.emitFromPath(ASW("table",5), strTable))
  {
    context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute",36), ASWNL("Missing module/table paramater"));
    return false;
  }

  AString str;
  context.useRequestParameterPairs().get(ASW("wikipath",8), str);

  AString query("select data from ",17);
  query.append(strTable);
  query.append(" where path='",13);
  query.append(str);
  query.append("'", 1);

  str.clear();
  AResultSet rs;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, rs, str);
  if (AConstant::npos == rows)
  {
    context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute",36),ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
    return false;
  }
  else if (!rows)
  {
    //a_Signal that the wiki file does not exist
    context.useOutputRootXmlElement().addElement(ASW("wiki/DoesNotExits",17));
  }
  else if (1 == rows)
  {
    //a_Publish it
    rs.emitXml(context.useOutputRootXmlElement().addElement(ASW("wiki",4)));
  }
  else
  {
    context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute",36), ASWNL("More than 1 entry for this path in the database, please fix the query to return only 1 entry"));
    return false;
  }

  return true;
}
