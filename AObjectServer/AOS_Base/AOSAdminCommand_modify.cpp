/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_modify.hpp"
#include "AXmlElement.hpp"

const AString AOSAdminCommand_modify::COMMAND("modify",6);

const AString& AOSAdminCommand_modify::getName() const
{
  return COMMAND;
}

AOSAdminCommand_modify::AOSAdminCommand_modify(AOSServices& services) :
  AOSAdminCommand_display(services)
{
}

void AOSAdminCommand_modify::_process(AOSAdminCommandContext& context)
{
  AString objectName;
  if (!context.useRequestHeader().useUrl().useParameterPairs().get(ASW("object",6), objectName))
  {
    context.useModel().useRoot().addElement(ASW("error",5), ASW("Please specify which 'object=' to modify",40));
  }
  else
  {
    //a_Display object specific list
    AOSAdminInterface *pAdminObject = m_Services.useAdminRegistry().getAdminObject(objectName);
    
    //a_Add methods if any
    if (pAdminObject)
    {
      AXmlElement& eObject = context.useModel().useRoot().addElement(ASW("object",6)).addAttribute(ASW("name",4), objectName);
      pAdminObject->adminProcessAction(eObject, context.useRequestHeader());
      pAdminObject->adminEmitXml(eObject, context.useRequestHeader());
    }
    else
    {
      context.useModel().useRoot().addElement(ASW("error",5), ASW("Unknown object",14));
    }
  }
}

//void AOSAdminCommand_modify::_insertStylesheet(AOSAdminCommandContext& context)
//{
//  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
//    .addAttribute(ASW("type",4), ASW("text/xsl",8))
//    .addAttribute(ASW("href",4), ASW("/xsl/_command/modify.xsl",24));
//}
