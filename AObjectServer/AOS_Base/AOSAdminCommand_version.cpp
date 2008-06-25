/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_version.hpp"
#include "AXmlElement.hpp"

const char _AOS_ADMIN_BUILD_INFO_[] = "AOS Admin \tBUILD(" __TIME__ " " __DATE__ ")";

const AString& AOSAdminCommand_version::getName() const
{
  static const AString VERSION("version",7);
  return VERSION;
}

AOSAdminCommand_version::AOSAdminCommand_version(AOSServices& services) :
  AOSAdminCommandInterface(services)
{
}

void AOSAdminCommand_version::_process(AOSAdminCommandContext& context)
{
  //a_Add version
  context.useModel().useRoot().addElement(ASW("version",7)).addData(ASWNL(_AOS_ADMIN_BUILD_INFO_));
}

void AOSAdminCommand_version::_insertStylesheet(AOSAdminCommandContext& context)
{
  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute(ASW("type",4), ASW("text/xsl",8))
    .addAttribute(ASW("href",4), ASW("/xsl/_command/version.xsl",25));
}
