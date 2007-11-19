#include "pchAOS_Wiki.hpp"
#include "AOSModule_WikiRemapPath.hpp"

const AString& AOSModule_WikiRemapPath::getClass() const
{
  static const AString CLASS("WikiRemapPath");
  return CLASS;
}

AOSModule_WikiRemapPath::AOSModule_WikiRemapPath(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_WikiRemapPath::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  const AXmlElement *pBasePath = moduleParams.findElement(ASW("base-path",9));
  if (!pBasePath)
  {
    context.addError(ASWNL("AOSModule_WikiRemapPath::execute"), ASWNL("missing 'module/base-path' parameter"));
    return false;
  }

  const AXmlElement *pWikiCommand = moduleParams.findElement(ASW("wiki-command",12));
  if (!pWikiCommand)
  {
    context.addError(ASWNL("AOSModule_WikiRemapPath::execute"), ASWNL("missing 'module/wiki-command' parameter"));
    return false;
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

  //a_Command to execute
  AString strWikiCommandPath;
  pWikiCommand->emitContent(strWikiCommandPath);

  //a_Check base path and add parameter with path
  AString strBasePath;
  pBasePath->emitContent(strBasePath);
  AString strPathAndFilename = context.useRequestUrl().getPathAndFilename();
  size_t startOfBase = strPathAndFilename.find(strBasePath);
  if (AConstant::npos == startOfBase)
  {
    context.addError(ASWNL("AOSModule_WikiRemapPath::execute"), ASWNL("Missing base path, is this executing in the correct path?"));
    return false;
  }

  //a_Set new command location and change URL path/filename
  if (!context.setNewCommandPath(strWikiCommandPath))
  {
    context.addError(ASWNL("AOSModule_WikiRemapPath::execute"), ARope("Unable to find the command for new request URL: ")+context.useRequestUrl());
    return false;
  }
  context.useModel().overwriteElement("wiki/command").addData(strWikiCommandPath);

    //a_Add new parameter with path
  AString strParam;
  strPathAndFilename.peek(strParam, startOfBase+strBasePath.getSize());
  context.useRequestUrl().useParameterPairs().insert(ASW("wikipath",8), strParam);

  return true;
}
