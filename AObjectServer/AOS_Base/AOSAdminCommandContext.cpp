/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommandContext.hpp"

const AString AOSAdminCommandContext::ELEMENT("admin",5);
const AString AOSAdminCommandContext::PARAM_XML("xml",3);

AOSAdminCommandContext::AOSAdminCommandContext(AHTTPRequestHeader& request, AHTTPResponseHeader& response, AOutputBuffer& output) :
  m_RequestHeader(request),
  m_ResponseHeader(response),
  m_Output(output),
  m_Model(ELEMENT)
{
}

AXmlDocument& AOSAdminCommandContext::useModel()
{
  return m_Model;
}

AHTTPRequestHeader& AOSAdminCommandContext::useRequestHeader()
{
  return m_RequestHeader;
}

AHTTPResponseHeader& AOSAdminCommandContext::useResponseHeader()
{
  return m_ResponseHeader;
}

AOutputBuffer& AOSAdminCommandContext::useOutput()
{
  return m_Output;
}

bool AOSAdminCommandContext::isForceXml() const
{
  return m_RequestHeader.useUrl().useParameterPairs().exists(PARAM_XML);
}