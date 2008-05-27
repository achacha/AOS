/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_InsertIntoModel.hpp"

const AString& AOSModule_InsertIntoModel::getClass() const
{
  static const AString CLASS("InsertIntoModel");
  return CLASS;
}

AOSModule_InsertIntoModel::AOSModule_InsertIntoModel(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_InsertIntoModel::execute(AOSContext& context, const AXmlElement& params)
{
  const AXmlElement *pDataNode = params.findElement(ASW("data",4));
  if (pDataNode)
  {
    AString strPath;
    params.emitString(ASW("path",4), strPath);
    context.useModel().addContent(pDataNode->clone(), strPath);
  }
  else
  {
    context.addError(ASWNL("AOSModule_InsertIntoModel::execute"), ASWNL("Required element /module/data not found."));
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}

