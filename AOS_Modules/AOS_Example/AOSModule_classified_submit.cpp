#include "pchAOS_Example.hpp"
#include "AOSModule_classified_submit.hpp"
#include "ADatabase.hpp"
#include "AResultSet.hpp"
#include "ATextConverter.hpp"

const AString& AOSModule_classified_submit::getClass() const
{
  static const AString CLASS("classified_submit");
  return CLASS;
}

AOSModule_classified_submit::AOSModule_classified_submit(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_classified_submit::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Start preparing an insert
  ARope query("INSERT INTO classified(title,contact,price,descript,user_id) VALUES ('");

  //a_Get input data
  AString str;
  
  // title
  if (context.useRequestParameterPairs().get("title", str))
  {
    query.append(str);
  }
  else
  {
    context.addError(ASWNL("AOSModule_classified_submit::execute"), ASWNL("Missing 'title' input"));
    return false;
  }
  query.append("','",3);

  // contact_email
  str.clear();
  if (context.useRequestParameterPairs().get("contact_email", str))
  {
    query.append(str);
  }
  else
  {
    context.addError(ASWNL("AOSModule_classified_submit::execute"), ASWNL("Missing 'contact_email' input"));
    return false;
  }
  query.append("','",3);

  // price
  str.clear();
  if (context.useRequestParameterPairs().get("price", str))
  {
    query.append(str);
  }
  query.append("','",3);

  // desc
  str.clear();
  if (context.useRequestParameterPairs().get("descript", str))
  {
    str.replace('\'', '`');
    ARope rope;
    ATextConverter::encodeURL(str, rope, true);
    query.append(rope);
  }
  else
  {
    context.addError(ASWNL("AOSModule_classified_submit::execute"), ASWNL("Missing 'descript' input"));
    return false;
  }
  query.append("',1)",4);

  //a_Commit to DB
  str.clear();
  AResultSet rs;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query.toAString(), rs, str);
  if (AConstant::npos == rows)
  {
    context.addError(ASW("AOSModule_classified_submit::execute",36),ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
    return false;
  }

  context.useModel().addElement("classified_submit").addData("Item added");
  return true;
}

