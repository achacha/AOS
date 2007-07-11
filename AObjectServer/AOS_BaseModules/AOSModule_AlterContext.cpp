#include "pchAOS_BaseModules.hpp"
#include "AOSModule_AlterContext.hpp"

const AString& AOSModule_AlterContext::getClass() const
{
  static const AString CLASS("AlterContext");
  return CLASS;
}

AOSModule_AlterContext::AOSModule_AlterContext(ALog& alog) :
  AOSModuleInterface(alog)
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
      (*cit)->emitFromPath("add-parameter/name", strName);
      (*cit)->emitFromPath("add-parameter/value", strValue);

      if (!strName.isEmpty())
      {
        context.useRequestParameterPairs().insert(strName, strValue);
      }
    }
  }

  nodes.clear();
  if (moduleParams.find(ASW("/params/module/add-cookie",25), nodes) > 0)
  {
    //a_Request parameters
    for (AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      AString strName;
      AString strValue;
      (*cit)->emitFromPath("add-cookie/name", strName);
      (*cit)->emitFromPath("add-cookie/value", strValue);

      if (!strName.isEmpty())
      {
        context.useRequestCookies().addCookie(strName, strValue);
      }
    }
  }

  return true;
}
