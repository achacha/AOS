/*
Written by Alex Chachanashvili

$Id$
*/
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

AOSContext::ReturnCode AOSModule_AlterContext::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  //a_Adding parameters
  AXmlElement::CONST_CONTAINER nodes;
  if (moduleParams.find(ASW("add-parameter",13), nodes) > 0)
  {
    //a_Request parameters
    for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitString("name", strName);
      (*cit)->emitString("value", strValue);

      if (!strName.isEmpty())
      {
        context.useRequestParameterPairs().insert(strName, strValue);
      }
    }
  }

  nodes.clear();
  if (moduleParams.find(ASW("add-request-cookie",18), nodes) > 0)
  {
    _processSetCookie(context.useRequestCookies(), nodes);
  }

  nodes.clear();
  if (moduleParams.find(ASW("add-response-cookie",19), nodes) > 0)
  {
    _processSetCookie(context.useResponseCookies(), nodes);
  }

  nodes.clear();
  if (moduleParams.find(ASW("add-request-header",18), nodes) > 0)
  {
    //a_Request header
    for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitString("name", strName);
      (*cit)->emitString("value", strValue);

      if (!strName.isEmpty())
      {
        context.useRequestHeader().setPair(strName, strValue);
      }
    }
  }

  nodes.clear();
  if (moduleParams.find(ASW("add-response-header",19), nodes) > 0)
  {
    //a_Response header
    for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitString("name", strName);
      (*cit)->emitString("value", strValue);

      if (!strName.isEmpty())
      {
        context.useResponseHeader().setPair(strName, strValue);
      }
    }
  }

  return AOSContext::RETURN_OK;
}

void AOSModule_AlterContext::_processSetCookie(ACookies& cookies, AXmlElement::CONST_CONTAINER& nodes)
{
  //a_Request parameters
  for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
  {
    AString strName;
    AString str;
    (*cit)->emitString("name", strName);
    (*cit)->emitString("value", str);

    if (!strName.isEmpty())
    {
      ACookie& cookie = cookies.addCookie(strName, str);

      str.clear();
      if ((*cit)->emitString("max-age", str))
      {
        if (!str.isEmpty())
          cookie.setMaxAge(str.toS4());

        str.clear();
      }

      if ((*cit)->emitString("expires", str))
      {
        ATime time;
        time.parseRFCtime(str);
        cookie.setExpires(time);
        str.clear();
      }

      if ((*cit)->emitString("path", str))
      {
        cookie.setPath(str);
        str.clear();
      }

      if ((*cit)->emitString("domain", str))
      {
        cookie.setDomain(str);
        str.clear();
      }

      if ((*cit)->emitString("version", str))
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