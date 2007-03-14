#include "pchModules.hpp"
#include "AOSModule_Admin.hpp"

const AString AOSModule_Admin::MODULE_NAME("AOS_Admin");

AOSModule_Admin::AOSModule_Admin(ALog& log) :
  AOSModuleInterface(log)
{
}

bool AOSModule_Admin::execute(AContext& context)
{
  context.useObjects().insert("/input/request_header", new AObject< AHTTPRequestHeader >(context.useRequestHeader(), AXmlData::CDataDirect));
  context.useObjects().insert("/output/response_header", new AObject< AHTTPResponseHeader >(context.useResponseHeader(), AXmlData::CDataDirect));
  context.useObjects().insert("/input/command", context.getCommand());
  
  //a_Check for authentication
  if (!__isAuthenticated(context))
    return false;

  //a_Process commands
  if (context.getCommand() == "admin_set")
  {
    __commandSetConfiguration(context);
  }
  
  return true;
}

bool AOSModule_Admin::__isAuthenticated(AContext& context)
{
  AString str;
  if (context.useRequestHeader().find(AHTTPRequestHeader::HT_REQ_Authorization, str))
  {
    int i = str.find("Basic");
    if (i == 0)
    {
      AString strX;
      str.peek(strX, 6);
      AString strPassword = ATextConverter::decodeBase64(strX);
      AObjectBase& obj = context.useObjects().insert("/request/authorization/");
      obj.useAttributes().insert("type", "Basic");
      context.useObjects().insert("/request/authorization/data", str, AXmlData::CDataDirect);
      
      AString strUser;
      strPassword.getUntil(strUser, ':');

      //a_Server security goes here :)
      if (strUser != "admin" && strPassword != "admin")
      {
        //a_Request a login
        context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_Unauthorized);
        context.useResponseHeader().setPair(AHTTPResponseHeader::HT_RES_WWW_Authenticate, "Basic realm=\"AObjectServer\"");
        context.useNamespace().insert("/output/command", "NOP");
      }
      else
        return true;  //a_Authenicated!
    }
    else
    {
      AObjectBase& obj = context.useNamespace().insert("/request/authorization/");
      obj.useAttributes().insert("type", "Unknown");
      context.useObjects().insert("/request/authorization/data", str, AXmlData::CDataDirect);
    }
  }
  else
  {
    //a_Request a login
    context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_Unauthorized);
    context.useResponseHeader().setPair(AHTTPResponseHeader::HT_RES_WWW_Authenticate, "Basic realm=\"AObjectServer\"");
    context.useObjects().insert("/output/command", "NOP");
  }
  return false;
}


void AOSModule_Admin::__commandSetConfiguration(AContext& context)
{
  AString str;
  str = context.useRequestHeader().useUrl().useQueryString().get("disableXslCache");
  if (!str.isEmpty())
  {   
    oneConfiguration.get().setConfig(AOSConfiguration::DisableXslCache, (str == "0" ? false : true));
    context.useObjects().insert("/configuration", new AOSConfiguration(oneConfiguration.get()));     //a_Insert a copy
  }
}
