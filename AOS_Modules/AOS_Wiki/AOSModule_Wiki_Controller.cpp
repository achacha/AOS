/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Wiki.hpp"
#include "AOSModule_Wiki_Controller.hpp"

const AString& AOSModule_Wiki_Controller::getClass() const
{
  static const AString CLASS("Wiki.Controller");
  return CLASS;
}

AOSModule_Wiki_Controller::AOSModule_Wiki_Controller(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_Wiki_Controller::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  const AXmlElement *pBasePath = moduleParams.findElement(ASW("base-path",9));
  if (!pBasePath)
  {
    context.addError(getClass(), ASWNL("missing 'module/base-path' parameter"));
    return AOSContext::RETURN_ERROR;
  }

  const AXmlElement *pWikiController = moduleParams.findElement(ASW("wiki-controller",15));
  if (!pWikiController)
  {
    context.addError(getClass(), ASWNL("missing 'module/wiki-controller' parameter"));
    return AOSContext::RETURN_ERROR;
  }

  if (context.useRequestParameterPairs().exists(ASW("wiki.edit",9)))
  {
    //a_Edit mode, add view path/filename
    AUrl editUrl(context.useRequestUrl());
    editUrl.useParameterPairs().remove(ASW("wiki.edit",9));
    context.useModel().overwriteElement(ASW("wiki/view-url",13)).addData(editUrl.getPathFileAndQueryString(), AXmlElement::ENC_CDATADIRECT);
  }
  else
  {
    //a_View mode, add edit path/filename
    AUrl editUrl(context.useRequestUrl());
    editUrl.useParameterPairs().insert(ASW("wiki.edit",9), ASW("1",1));
    context.useModel().overwriteElement(ASW("wiki/edit-url",13)).addData(editUrl.getPathFileAndQueryString(), AXmlElement::ENC_CDATADIRECT);
  }

  //a_Controller to execute
  AString strWikiControllerPath;
  pWikiController->emitContent(strWikiControllerPath);

  //a_Check base path and add parameter with path
  AString strBasePath;
  pBasePath->emitContent(strBasePath);
  AString strPathAndFilename = context.useRequestUrl().getPathAndFilename();
  size_t startOfBase = strPathAndFilename.find(strBasePath);
  if (AConstant::npos == startOfBase)
  {
    context.addError(getClass(), ASWNL("Missing base path, is this executing in the correct path?"));
    return AOSContext::RETURN_ERROR;
  }

  //a_Set new command location and change URL path/filename
  if (!context.setNewControllerPath(strWikiControllerPath))
  {
    context.addError(getClass(), ARope("Unable to find the command for new request URL: ")+context.useRequestUrl());
    return AOSContext::RETURN_ERROR;
  }
  context.useModel().overwriteElement("wiki/command").addData(strWikiControllerPath);

    //a_Add new parameter with path
  AString strParam;
  strPathAndFilename.peek(strParam, startOfBase+strBasePath.getSize());
  context.useRequestUrl().useParameterPairs().insert(ASW("wikipath",8), strParam);

  return AOSContext::RETURN_OK;
}
