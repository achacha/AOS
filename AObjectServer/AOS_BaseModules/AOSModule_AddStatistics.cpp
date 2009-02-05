/*
Written by Alex Chachanashvili

$Id: AOSModule_AddStatistics.cpp 315 2008-12-01 14:05:00Z achacha $
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_AddStatistics.hpp"

const AString AOSModule_AddStatistics::CLASS("AddStatistics");

const AString& AOSModule_AddStatistics::getClass() const
{
  return CLASS;
}

AOSModule_AddStatistics::AOSModule_AddStatistics(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_AddStatistics::execute(AOSContext& context, const AXmlElement& moduleParams)
{
  context.useModel().addElement(ASW("uptime",6), m_Services.getUptimeTimer());
  return AOSContext::RETURN_OK;
}

