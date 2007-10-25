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

  AString strBasePath;
  pBasePath->emitContent(strBasePath);
  AString strPathAndFilename = context.useRequestUrl().getPathAndFilename();
  size_t startOfBase = strPathAndFilename.find(strBasePath);
  if (AConstant::npos == startOfBase)
  {
    context.addError(ASWNL("AOSModule_WikiRemapPath::execute"), ASWNL("Missing base path, is this executig in the correct path?"));
    return false;
  }

  //a_Set new command location
  AString strWikiCommand;
  pWikiCommand->emitContent(strWikiCommand);
  context.useRequestUrl().setPathAndFilename(strWikiCommand);

  //a_Add new parameter with path
  AString strParam;
  strPathAndFilename.peek(strParam, startOfBase+strBasePath.getSize());
  context.useRequestUrl().useParameterPairs().insert(ASW("wikipath",8), strParam);
  if (!context.setCommandFromRequestUrl())
  {
    context.addError(ASWNL("AOSModule_WikiRemapPath::execute"), ARope("Unable to find the command for new request URL: ")+context.useRequestUrl());
    return false;
  }

  return true;
}
