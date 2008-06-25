/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_list.hpp"
#include "AXmlElement.hpp"

const AString& AOSAdminCommand_list::getName() const
{
  static const AString VERSION("list",4);
  return VERSION;
}

AOSAdminCommand_list::AOSAdminCommand_list(AOSServices& services) :
  AOSAdminCommandInterface(services)
{
}

void AOSAdminCommand_list::_process(AOSAdminCommandContext& context)
{
  //a_Display list of objects
  LIST_AString objectNames;
  m_Services.useAdminRegistry().listAdminObjects(objectNames);

  LIST_AString::iterator it = objectNames.begin();
  while (it != objectNames.end())
  {
    AXmlElement& e = context.useModel().useRoot().addElement(ASW("object",6));
    e.addAttribute(ASW("name",4), *it);

    ++it;
  }
}

void AOSAdminCommand_list::_insertStylesheet(AOSAdminCommandContext& context)
{
  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute(ASW("type",4), ASW("text/xsl",8))
    .addAttribute(ASW("href",4), ASW("/xsl/_command/list.xsl",22));
}
