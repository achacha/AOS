/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Blog.hpp"
#include "AOSModule_BlogView.hpp"
#include "AOSDatabaseConnectionPool.hpp"
#include "AResultSet.hpp"
#include "ATextConverter.hpp"

const AString AOSModule_BlogView::CLASS("Blog.view");

const AString& AOSModule_BlogView::getClass() const
{
  return CLASS;
}

AOSModule_BlogView::AOSModule_BlogView(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_BlogView::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  AXmlElement& xmlBlog = context.useModel().addElement("blog");
  AString userId;
  if (!context.useRequestParameterPairs().get(ASW("userid",6), userId))
  {
    //a_No user parameter
    xmlBlog.addElement(ASW("error",5)).addElement(ASW("no-userid",9));
    return AOSContext::RETURN_OK;
  }

  AString query("select title, data, created from blog where user_id=");
  ATextConverter::makeSQLSafe(userId, query);  // Makes the user input SQL safe (to prevent SQL injection)

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
