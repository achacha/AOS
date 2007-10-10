#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_JSON.hpp"

const AString& AOSOutput_JSON::getClass() const
{
  static const AString CLASS("JSON");
  return CLASS;
}

AOSOutput_JSON::AOSOutput_JSON(ALog& alog) :
  AOSOutputGeneratorInterface(alog)
{
}

bool AOSOutput_JSON::execute(AOSOutputContext& context)
{
  context.useOutputRootXmlElement().emitJson(context.useOutputBuffer(),0);
  context.useResponseHeader().setPair(AHTTPResponseHeader::HT_ENT_Content_Type, ASW("text/plain",10));

  return true;
}
