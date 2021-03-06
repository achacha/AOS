/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Classified.hpp"
#include "AOSModule_classified_submit.hpp"
#include "ADatabase.hpp"
#include "AResultSet.hpp"
#include "ATextConverter.hpp"

const AString AOSModule_classified_submit::CLASS("Classified.submit");

const AString& AOSModule_classified_submit::getClass() const
{
  return CLASS;
}

AOSModule_classified_submit::AOSModule_classified_submit(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_classified_submit::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Start preparing an insert
  ARope query("INSERT INTO classified(title,contact,price,descript,user_id) VALUES ('");

  //a_Get input data
  AString str;
  
  // title
  if (context.useRequestParameterPairs().get("title", str))
  {
    if (str.isNotEmpty())
    {
      //a_Encode URL to hide any special characters (this is an alternative to makeSQLSafe)
      AString strSafe;
      ATextConverter::makeSQLSafe(str, strSafe);
      ATextConverter::encodeURL(strSafe, query);
    }
    else
    {
      context.addError(getClass(), ASWNL("Empty 'title' input"));
      return AOSContext::RETURN_ERROR;
    }
  }
  else
  {
    context.addError(getClass(), ASWNL("Missing 'title' input"));
    return AOSContext::RETURN_ERROR;
  }
  query.append("','",3);

  // contact_email
  str.clear();
  if (context.useRequestParameterPairs().get("contact_email", str))
  {
    if (str.isNotEmpty())
    {
      //a_Encode URL to hide any special characters
      AString strSafe;
      ATextConverter::makeSQLSafe(str, strSafe);
      ATextConverter::encodeURL(strSafe, query);
    }
    else
    {
      context.addError(getClass(), ASWNL("Empty 'contact_email' input"));
      return AOSContext::RETURN_ERROR;
    }
  }
  else
  {
    context.addError(getClass(), ASWNL("Missing 'contact_email' input"));
    return AOSContext::RETURN_ERROR;
  }
  query.append("','",3);

  // price
  str.clear();
  if (context.useRequestParameterPairs().get("price", str) && str.isNotEmpty())
  {
    //a_Encode URL to hide any special characters
    AString strSafe;
    ATextConverter::makeSQLSafe(str, strSafe);
    ATextConverter::encodeURL(strSafe, query);
  }
  query.append("','",3);

  // desc
  str.clear();
  if (context.useRequestParameterPairs().get("descript", str) && str.isNotEmpty())
  {
    //a_Encode URL to hide any special characters
    AString strSafe;
    ATextConverter::makeSQLSafe(str, strSafe);
    ATextConverter::encodeURL(strSafe, query);
  }
  else
  {
    context.addError(getClass(), ASWNL("Missing 'descript' input"));
    return AOSContext::RETURN_ERROR;
  }
  query.append("',1)",4);

  //a_Commit to DB
  str.clear();
  AResultSet rs;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query.toAString(), rs, str);
  if (AConstant::npos == rows)
  {
    context.addError(getClass(), ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
    return AOSContext::RETURN_ERROR;
  }

  context.useModel().addElement("classified_submit").addData("Item added");
  return AOSContext::RETURN_OK;
}

