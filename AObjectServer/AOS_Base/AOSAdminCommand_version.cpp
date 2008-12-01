/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_version.hpp"
#include "apiAGdLib.hpp"
#include "apiALuaEmbed.hpp"
#include "apiAZlib.hpp"
#include "apiADatabase_SQLite.hpp"
#include "apiADatabase_MySQL.hpp"
#include "apiADatabase_ODBC.hpp"
#include "apiACrypto.hpp"

const char _AOS_ADMIN_BUILD_INFO_[] = "AOS Admin \tBUILD(" __TIME__ " " __DATE__ ")";

const AString AOSAdminCommand_version::COMMAND("version",7);

const AString& AOSAdminCommand_version::getName() const
{
  return COMMAND;
}

AOSAdminCommand_version::AOSAdminCommand_version(AOSServices& services) :
  AOSAdminCommandInterface(services)
{
}

void AOSAdminCommand_version::_process(AOSAdminCommandContext& context)
{
  //a_Add versions
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(AOS_Base_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(_AOS_ADMIN_BUILD_INFO_));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(ABase_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(ALuaEmbed_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(AZlib_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(GDLib_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(ADatabase_SQLite_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(ADatabase_MySQL_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(ADatabase_ODBC_INFO));
  context.useModel().useRoot().addElement(COMMAND).addData(ASWNL(ACrypto_INFO));
}

void AOSAdminCommand_version::_insertStylesheet(AOSAdminCommandContext& context)
{
  context.useModel().addInstruction(AXmlInstruction::XML_STYLESHEET)
    .addAttribute(ASW("type",4), ASW("text/xsl",8))
    .addAttribute(ASW("href",4), ASW("/xsl/_command/version.xsl",25));
}
