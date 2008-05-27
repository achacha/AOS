/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSAdminRegistry.hpp"

const AString AOSAdminInterface::DEBUGDUMP("debugDump",9);
const AString AOSAdminInterface::NAME("name",4);
const AString AOSAdminInterface::PROPERTY("property",8);
const AString AOSAdminInterface::VALUE("value",5);
const AString AOSAdminInterface::ACTION("action",6);
const AString AOSAdminInterface::DESC("desc",4);
const AString AOSAdminInterface::PARAM("param",5);
const AString AOSAdminInterface::ADMIN_ERROR("error",5);
const AString AOSAdminInterface::ADMIN_MESSAGE("message",7);

AXmlElement& AOSAdminInterface::adminAddProperty(
  AXmlElement& eBase,
  const AString& propertyName,
  const AEmittable& propertyValue,
  AXmlElement::Encoding propertyValueEncoding // = AXmlElement::ENC_NONE
) const
{
  AXmlElement& eProperty = eBase.addElement(PROPERTY).addAttribute(NAME, propertyName);
  eProperty.addElement(VALUE, propertyValue, propertyValueEncoding);
  return eProperty;
}

AXmlElement& AOSAdminInterface::adminAddActionToProperty(
  AXmlElement& eProperty,
  const AString& actionName,
  const AString& actionDesc
) const
{
  AXmlElement& eAction = eProperty.addElement(ACTION).addAttribute(NAME, actionName).addAttribute(DESC, actionDesc);
  return eAction;
}

AXmlElement& AOSAdminInterface::adminAddActionToProperty(
  AXmlElement& eProperty,
  const AString& actionName,
  const AString& actionDesc,
  const AString& actionParam
) const
{
  AXmlElement& eAction = eProperty.addElement(ACTION).addAttribute(NAME, actionName).addAttribute(DESC, actionDesc);
  eAction.addElement(PARAM, actionParam);
  return eAction;
}

AXmlElement& AOSAdminInterface::adminAddActionToProperty(
  AXmlElement& eProperty,
  const AString& actionName,
  const AString& actionDesc,
  const LIST_AString& actionParams
) const
{
  AXmlElement& eAction = eProperty.addElement(ACTION).addAttribute(NAME, actionName).addAttribute(DESC, actionDesc);
  for (LIST_AString::const_iterator cit = actionParams.begin(); cit != actionParams.end(); ++cit)
    eAction.addElement(PARAM, *cit);
  return eAction;
}

AXmlElement& AOSAdminInterface::adminAddPropertyWithAction(
  AXmlElement& eBase, 
  const AString& propertyName, 
  const AEmittable& propertyValue,
  const AString& actionName,
  const AString& actionDesc
) const
{
  AXmlElement& eProperty = adminAddProperty(eBase, propertyName, propertyValue);
  adminAddActionToProperty(eProperty, actionName, actionDesc);
  return eProperty;
}

AXmlElement& AOSAdminInterface::adminAddPropertyWithAction(
  AXmlElement& eBase, 
  const AString& propertyName, 
  const AEmittable& propertyValue,
  const AString& actionName,
  const AString& actionDesc,
  const AString& param
) const
{
  AXmlElement& eProperty = adminAddProperty(eBase, propertyName, propertyValue);
  adminAddActionToProperty(eProperty, actionName, actionDesc, param);
  return eProperty;
}

AXmlElement& AOSAdminInterface::adminAddPropertyWithAction(
  AXmlElement& eBase, 
  const AString& propertyName, 
  const AEmittable& propertyValue,
  const AString& actionName,
  const AString& actionDesc,
  const LIST_AString& params
) const
{
  AXmlElement& eProperty = adminAddProperty(eBase, propertyName, propertyValue);
  adminAddActionToProperty(eProperty, actionName, actionDesc, params);
  return eProperty;
}

AXmlElement& AOSAdminInterface::adminAddError(
  AXmlElement& eBase, 
  const AString& adminError,
  AXmlElement::Encoding adminErrorEncoding //= AXmlElement::CDataDirect
) const
{
  return eBase.addElement(ADMIN_ERROR, adminError, adminErrorEncoding);
}

AXmlElement& AOSAdminInterface::adminAddMessage(
  AXmlElement& eBase, 
  const AString& adminMessage,
  AXmlElement::Encoding adminMessageEncoding //= AXmlElement::CDataDirect
) const
{
  return eBase.addElement(ADMIN_MESSAGE, adminMessage, adminMessageEncoding);
}

void AOSAdminInterface::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  ARope rope;
  if (request.getUrl().getParameterPairs().exists(DEBUGDUMP))
  {
    debugDumpToAOutputBuffer(rope);
    eBase.addElement(DEBUGDUMP, rope, AXmlElement::ENC_CDATASAFE);
  }
}

void AOSAdminInterface::adminRegisterObject(AOSAdminRegistry& registry)
{
  registry.insert(getClass(), *this);
}

void AOSAdminInterface::adminRegisterObject(AOSAdminRegistry& registry, const AString& parentClassName)
{
  registry.insert(parentClassName+ASW(".",1)+getClass(), *this);
}

void AOSAdminInterface::adminProcessAction(AXmlElement&, const AHTTPRequestHeader&)
{
}

void AOSAdminInterface::addExecutionTimeSample(double milliseconds)
{
  m_ExecutionTimeAverage.addSample(milliseconds);
}

u8 AOSAdminInterface::getExecutionCount() const
{
  return m_ExecutionTimeAverage.getCount();
}

double AOSAdminInterface::getExecutionTimeAverage() const
{ 
  return m_ExecutionTimeAverage.getAverage(); 
}
