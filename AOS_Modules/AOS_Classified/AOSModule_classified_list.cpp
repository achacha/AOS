#include "pchAOS_Classified.hpp"
#include "AOSModule_classified_list.hpp"
#include "ADatabase.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_classified_list::getClass() const
{
  static const AString CLASS("classified_list");
  return CLASS;
}

AOSModule_classified_list::AOSModule_classified_list(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_classified_list::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Start preparing an insert
  ARope query("SELECT * from classified where user_id=1;");

  //a_Commit to DB
  AString str;
  AResultSet rs;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query.toAString(), rs, str);
  if (AConstant::npos == rows)
  {
    context.addError(ASWNL("AOSModule_classified_list::execute"), str);
    context.addError(ASWNL("AOSModule_classified_list::execute.query"), query);
    return false;
  }

  AXmlElement& eClassified = context.useModel().addElement("classified");
  rs.emitXml(eClassified);

  return true;
}

