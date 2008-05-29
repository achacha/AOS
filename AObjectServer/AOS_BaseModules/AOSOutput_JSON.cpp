/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_JSON.hpp"

const AString& AOSOutput_JSON::getClass() const
{
  static const AString CLASS("JSON");
  return CLASS;
}

AOSOutput_JSON::AOSOutput_JSON(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_JSON::execute(AOSContext& context)
{
  context.useModel().emitJson(context.useOutputBuffer(),0);
  context.useResponseHeader().setPair(AHTTPResponseHeader::HT_ENT_Content_Type, ASW("text/plain",10));

  return AOSContext::RETURN_OK;
}
