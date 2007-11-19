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

  AString strWikiPath;
  context.useRequestParameterPairs().get(ASW("wikipath",8), strWikiPath);

  AString strData;
  if (context.useRequestParameterPairs().get(ASW("wiki.newdata",12), strData))
  {
    //a_Does it exist
    AString query("select id from ",15);
    query.append(strTable);
    query.append(" where path='",13);
    query.append(strWikiPath);
    query.append("'", 1);

    AString strError;
    AResultSet rs;
    size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, rs, strError);
    if (AConstant::npos == rows)
    {
      //a_INSERT new data for this page, write data
      AString query("insert into",11);
      query.append(strTable);
      query.append(" (path,data) values ('",22);
      query.append(strWikiPath);
      query.append("','",3);
      query.append(strData);
      query.append("')",2);
      
      AString str;
      AResultSet rs;
      size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, rs, str);
      if (AConstant::npos == rows)
      {
        context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute:INSERT:"),ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
        return false;
      }
    }
    else
    {
      //a_UPDATE existing data
      AString query("update ",7);
      query.append(strTable);
      query.append(" set data='",11);
      query.append(strData);
      query.append("' where path='",14);
      query.append(strWikiPath);
      query.append("'",1);
      
      AString str;
      AResultSet rs;
      size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, rs, str);
      if (AConstant::npos == rows)
      {
        context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute:UPDATE:"),ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
        return false;
      }
    }

    //a_Publish it
    context.useModel().overwriteElement(ASW("wiki/row/data",13)).addData(strData, AXmlElement::ENC_CDATADIRECT);
  }
  else
  {
    //a_Read from database
    AString query("select data from ",17);
    query.append(strTable);
    query.append(" where path='",13);
    query.append(strWikiPath);
    query.append("'", 1);

    AString str;
    AResultSet rs;
    size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, rs, str);
    if (AConstant::npos == rows)
    {
      context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute:SELECT:"),ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
      return false;
    }
    else if (!rows)
    {
      //a_Signal that the wiki file does not exist
      context.useModel().overwriteElement(ASW("wiki/DoesNotExist",17));
    }
    else if (1 == rows)
    {
      //a_Publish it
      rs.emitXml(context.useModel().overwriteElement(ASW("wiki",4)));
    }
    else
    {
      context.addError(ASWNL("AOSModule_WikiViewFromDatabase::execute:SELECT:"), ASWNL("More than 1 entry for this path in the database, please fix the query to return only 1 entry"));
      return false;
    }
  }

  return true;
}
