#include "pchAOS_Base.hpp"
#include "AOSOutputContext.hpp"
#include "AOSServices.hpp"

#ifdef __DEBUG_DUMP__
void AOSOutputContext::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSOutputContext @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "&m_Context=" << std::hex << (void *)&m_Context << std::dec << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSOutputContext::AOSOutputContext(
  AOSContext& context
) :
  m_Context(context)
{
}

AOSOutputContext::~AOSOutputContext()
{
}

void AOSOutputContext::emit(AXmlElement& target) const
{
  m_Context.emit(target);
}

const AObjectContainer& AOSOutputContext::getContextObjects() const
{
  return m_Context.useContextObjects();
}

ARope& AOSOutputContext::useOutputBuffer()
{
  return m_Context.useOutputBuffer();
}

const AXmlDocument& AOSOutputContext::getOutputXmlDocument()const
{
  return m_Context.useOutputXmlDocument();
}

const AXmlElement& AOSOutputContext::getOutputRootXmlElement() const
{
  return m_Context.useOutputXmlDocument().useRoot();
}

AString AOSOutputContext::getOutputCommand() const
{
  return m_Context.getOutputCommand();
}

void AOSOutputContext::setResponseMimeTypeFromRequestExtension()
{
  m_Context.setResponseMimeTypeFromRequestExtension();
}

const AHTTPRequestHeader& AOSOutputContext::getRequestHeader() const
{
  return m_Context.useRequestHeader();
}

const ACookies& AOSOutputContext::getRequestCookies() const
{ 
  return m_Context.useRequestHeader().useCookies();
}

const AUrl& AOSOutputContext::getRequestUrl() const
{ 
  return m_Context.useRequestHeader().useUrl();
}

const AQueryString& AOSOutputContext::getRequestParameterPairs() const
{ 
  return m_Context.useRequestHeader().useUrl().useParameterPairs();
}

AHTTPResponseHeader& AOSOutputContext::useResponseHeader()
{
  return m_Context.useResponseHeader();
}

ACookies& AOSOutputContext::useResponseCookies()
{
  return m_Context.useResponseHeader().useCookies();
}

const AXmlElement& AOSOutputContext::getOutputParams() const
{
  return m_Context.getOutputParams();
}

const AOSConfiguration& AOSOutputContext::getConfiguration() const
{
  return m_Context.useServices().useConfiguration();
}

void AOSOutputContext::addError(const AString& where, const AString& what)
{
  m_Context.addError(where, what);
}

const AStringHashMap& AOSOutputContext::getSessionData() const
{
  return m_Context.useSessionData();
}

AEventVisitor& AOSOutputContext::useEventVisitor()
{
  return m_Context.useEventVisitor();
}

AFile_Socket& AOSOutputContext::useSocket()
{
  return m_Context.useSocket();
}

bool AOSOutputContext::isOutputCommitted() const
{
  return m_Context.useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
}

void AOSOutputContext::setOutputCommitted(bool b)
{
  m_Context.useContextFlags().setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT, b);
}
