#include "pchAOS_BaseModules.hpp"
#include "AOSModule_AlterContext.hpp"

const AString& AOSModule_AlterContext::getClass() const
{
  static const AString CLASS("AlterContext");
  return CLASS;
}

AOSModule_AlterContext::AOSModule_AlterContext(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_AlterContext::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Adding parameters
  AXmlNode::ConstNodeContainer nodes;
  if (moduleParams.find(ASW("/params/module/add-parameter",28), nodes) > 0)
  {
    //a_Request parameters
    for (AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitFromPath("name", strName);
      (*cit)->emitFromPath("value", strValue);

      if (!strName.isEmpty())
      {
        context.useRequestParameterPairs().insert(strName, strValue);
      }
    }
  }

  nodes.clear();
  if (moduleParams.find(ASW("/params/module/add-cookie",25), nodes) > 0)
  {
    _processSetCookie(context.useRequestCookies(), nodes);
  }

  nodes.clear();
  if (moduleParams.find(ASW("/params/module/add-set-cookie",29), nodes) > 0)
  {
    _processSetCookie(context.useResponseCookies(), nodes);
  }

  nodes.clear();
  if (moduleParams.find(ASW("/params/module/add-request-header",33), nodes) > 0)
  {
    //a_Request header
    for (AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitFromPath("name", strName);
      (*cit)->emitFromPath("value", strValue);

      if (!strName.isEmpty())
      {
        context.useRequestHeader().setPair(strName, strValue);
      }
    }
  }

  nodes.clear();
  if (moduleParams.find(ASW("/params/module/add-response-header",34), nodes) > 0)
  {
    //a_Response header
    for (AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitFromPath("name", strName);
      (*cit)->emitFromPath("value", strValue);

      if (!strName.isEmpty())
      {
        context.useResponseHeader().setPair(strName, strValue);
      }
    }
  }

  return true;
}

void AOSModule_AlterContext::_processSetCookie(ACookies& cookies, AXmlNode::ConstNodeContainer& nodes)
{
  //a_Request parameters
  for (AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
  {
    AString strName;
    AString str;
    (*cit)->emitFromPath("name", strName);
    (*cit)->emitFromPath("value", str);

    if (!strName.isEmpty())
    {
      ACookie& cookie = cookies.addCookie(strName, str);

      str.clear();
      if ((*cit)->emitFromPath("maxage", str))
      {
        if (!str.isEmpty())
          cookie.setMaxAge(str.toS4());

        str.clear();
      }

      if ((*cit)->emitFromPath("expires", str))
      {
        ATime time;
        time.parseRFCtime(str);
        cookie.setExpires(time);
        str.clear();
      }

      if ((*cit)->emitFromPath("path", str))
      {
        cookie.setPath(str);
        str.clear();
      }

      if ((*cit)->emitFromPath("domain", str))
      {
        cookie.setDomain(str);
        str.clear();
      }

      if ((*cit)->emitFromPath("version", str))
      {
        cookie.setDomain(str);
        str.clear();
      }

      if ((*cit)->exists("expired"))
      {
        cookie.setExpired();
      }

      if ((*cit)->exists("no-expire"))
      {
        cookie.setNoExpire();
      }

      if ((*cit)->exists("secure"))
      {
        cookie.setSecure();
      }
    }
  }
}