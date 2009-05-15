/*
Written by Alex Chachanashvili

$Id: AOSContext.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSContextUtils.hpp"
#include "AXmlElement.hpp"

const AString AOSContextUtils::ATTR_REFERENCE("reference",9);

bool AOSContextUtils::getContentWithReference(AOSContext& context, const AXmlElement& element, AOutputBuffer& result)
{
  AString str;
  if (element.getAttributes().get(AOSContextUtils::ATTR_REFERENCE, str))
  {
    if (str.equals(AOSContext::S_SESSION))
      return context.useSessionData().useData().emitContentFromPath(str, result);
    else if (str.equals(AOSContext::S_MODEL))
      return context.useModel().emitContentFromPath(str, result);
    else if (str.equals(AOSContext::S_REFERER))
      return (context.useRequestHeader().get(AHTTPHeader::HT_REQ_Referer, result) > 0);
    else
      return false;
  }

  element.emitContent(result);
  return true;
}
