#include "pchAOS_Classified.hpp"
#include "AOSModule_classified_list.hpp"
#include "ADatabase.hpp"
#include "AResultSet.hpp"
#include "ATextConverter.hpp"

const AString& AOSModule_classified_list::getClass() const
{
  static const AString CLASS("classified_list");
  return CLASS;
}

AOSModule_classified_list::AOSModule_classified_list(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_classified_list::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Start preparing an insert
  ARope query("SELECT * from classified where user_id=1;");

  //a_Commit to DB
  AString str(4096, 4096);
  AResultSet rs;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query.toAString(), rs, str);
  if (AConstant::npos == rows)
  {
    context.addError(getClass(), query);
    context.addError(getClass(), str);
    return AOSContext::RETURN_ERROR;
  }

  AXmlElement& eClassified = context.useModel().addElement("classified");
  eClassified.addElement("sql").addData(rs.useSQL(), AXmlElement::ENC_CDATADIRECT);

  str.clear();
  const VECTOR_AString& fieldNames = rs.getFieldNames();
  for (size_t i = 0; i < rs.getRowCount(); ++i)
  {
    AXmlElement& row = eClassified.addElement(ASW("row",3));
    const VECTOR_AString& fieldData = rs.getRow(i);
    for (size_t j = 0; j < fieldNames.size(); ++j)
    {
      //a_Decode URL the data which was encoded during insert
      ATextConverter::decodeURL(fieldData.at(j), str);
      row.addElement(fieldNames.at(j)).addData(str, AXmlElement::ENC_CDATADIRECT);
      str.clear();
    }
  }

  return AOSContext::RETURN_OK;
}

