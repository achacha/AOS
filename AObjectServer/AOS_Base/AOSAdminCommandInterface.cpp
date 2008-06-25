/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommandInterface.hpp"

AOSAdminCommandInterface::AOSAdminCommandInterface(AOSServices& services) :
  m_Services(services)
{
}

AOSAdminCommandInterface::~AOSAdminCommandInterface()
{
}

void AOSAdminCommandInterface::process(AOSAdminCommandContext& context)
{
  _process(context);
  
  //a_Stylesheet
  if (!context.isForceXml())
  {
    _insertStylesheet(context);
  }

  //a_Output
  context.useModel().emit(context.useOutput());
}
