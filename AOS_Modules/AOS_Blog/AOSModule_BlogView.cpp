#include "pchAOS_Blog.hpp"
#include "AOSModule_BlogView.hpp"
#include "AOSDatabaseConnectionPool.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_BlogView::getClass() const
{
  static const AString CLASS("BlogView");
  return CLASS;
}

AOSModule_BlogView::AOSModule_BlogView(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_BlogView::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& xmlBlog = context.useOutputRootXmlElement().addElement("blog");
  
  AString userId("1");
  static const AString queryBase("select title, data, created from blog where user_id=");
  AString query(queryBase);
  query.append(userId);

  AString error;
  AResultSet result;
  size_t rows = context.useServices().useDatabaseConnectionPool().useDatabasePool().executeSQL(query, result, error);
  if (rows > 0 && error.isEmpty())
  {
    result.emitXml(xmlBlog);
  }
  else
  {
    context.addError(this->getClass(), error);
  }
  return true;
}
