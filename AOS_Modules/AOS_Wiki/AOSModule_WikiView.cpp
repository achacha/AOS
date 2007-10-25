#include "pchAOS_Wiki.hpp"
#include "AOSModule_WikiView.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_WikiView::getClass() const
{
  static const AString CLASS("WikiView");
  return CLASS;
}

AOSModule_WikiView::AOSModule_WikiView(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_WikiView::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AString str;
  context.useRequestParameterPairs().get(ASW("wikipath",8), str);
  
  AString query("select data from wiki where path='",34);
  query.append(str);
  query.append("'", 1);

  str.clear();
  AResultSet rs;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, rs, str);
  if (AConstant::npos == rows)
  {
    context.addError(ASWNL("AOSModule_WikiView::execute",36),ARope("[",1)+query+ASW("]{",2)+str+ASW("}",1));
    return false;
  }
  else
  {
    rs.emitXml(context.useOutputRootXmlElement().addElement(ASW("wiki",4)));
  }

  return true;
}
