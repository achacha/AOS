/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_display.hpp"
#include "AXmlElement.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"

const AString AOSAdminCommand_display::COMMAND("display",7);

const AString& AOSAdminCommand_display::getName() const
{
  return COMMAND;
}

AOSAdminCommand_display::AOSAdminCommand_display(AOSServices& services) :
  AOSAdminCommandInterface(services)
{
}

void AOSAdminCommand_display::_process(AOSAdminCommandContext& context)
{
  AString objectName;
  if (!context.useRequestHeader().useUrl().useParameterPairs().get(ASW("object",6), objectName))
  {
    context.useModel().useRoot().addElement(ASW("error",5), ASW("Please specify which 'object=' to display",41));
  }
  else
  {
    //a_Display object specific list
    AOSAdminInterface *pAdminObject = m_Services.useAdminRegistry().getAdminObject(objectName);
    
    //a_Add methods if any
    if (pAdminObject)
    {
      AXmlElement& eObject = context.useModel().useRoot().addElement(ASW("object",6)).addAttribute(ASW("name",4), objectName);
      pAdminObject->adminEmitXml(eObject, context.useRequestHeader());
    }
    else
    {
      context.useModel().useRoot().addElement(ASW("error",5), ASW("Unknown object",14));
    }
  }
}

void AOSAdminCommand_display::_insertStylesheet(AOSAdminCommandContext& context)
{
  AString stylesheet("/xsl/_command/display",21);
  AString objectName;
  if (context.useRequestHeader().useUrl().useParameterPairs().get("object", objectName))
  {
    stylesheet.append('_');
    stylesheet.append(objectName);
  }
  stylesheet.append(".xsl",4);

  AFilename fn(m_Services.useConfiguration().getAdminBaseHttpDir(), stylesheet, false);
  if (!AFileSystem::exists(fn))
  {
    //a_Fallback on default
    stylesheet.assign("/xsl/_command/display.xsl",25);
  }

  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute(ASW("type",4), ASW("text/xsl",8))
    .addAttribute(ASW("href",4), stylesheet);
}
