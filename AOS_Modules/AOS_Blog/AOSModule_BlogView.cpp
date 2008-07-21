/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Blog.hpp"
#include "AOSModule_BlogView.hpp"
#include "AOSDatabaseConnectionPool.hpp"
#include "AResultSet.hpp"

const AString& AOSModule_BlogView::getClass() const
{
  static const AString CLASS("Blog.view");
  return CLASS;
}

AOSModule_BlogView::AOSModule_BlogView(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_BlogView::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& xmlBlog = context.useModel().addElement("blog");
  
  AString userId("1");
  AString query("select title, data, created from blog where user_id=");
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
  return AOSContext::RETURN_OK;
}
