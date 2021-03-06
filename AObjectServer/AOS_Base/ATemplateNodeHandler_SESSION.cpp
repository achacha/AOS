/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "ATemplateNodeHandler_SESSION.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "AOSContext.hpp"
#include "AEventVisitor.hpp"

const AString ATemplateNodeHandler_SESSION::TAGNAME("aos:session");

void ATemplateNodeHandler_SESSION::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateNodeHandler_SESSION::ATemplateNodeHandler_SESSION()
{
}

ATemplateNodeHandler_SESSION::~ATemplateNodeHandler_SESSION()
{
}

void ATemplateNodeHandler_SESSION::init()
{
}

void ATemplateNodeHandler_SESSION::deinit()
{
}

const AString& ATemplateNodeHandler_SESSION::getTagName() const
{
  return ATemplateNodeHandler_SESSION::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_SESSION::create(AFile *pFile)
{
  ATemplateNodeHandler_SESSION::Node *pNode = new ATemplateNodeHandler_SESSION::Node(this);
  
  if (pFile)
    pNode->fromAFile(*pFile);
  
  return pNode;
}

///////////////////////////////////////// ATemplateNodeHandler_SESSION::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_SESSION::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_SESSION::Node::Node(const ATemplateNodeHandler_SESSION::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_SESSION::Node::~Node()
{
}

void ATemplateNodeHandler_SESSION::Node::process(ATemplateContext& ctx, AOutputBuffer& output)
{
  AAutoPtr<AEventVisitor::ScopedEvent> scoped;
  if (ctx.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
  {
    scoped.reset(new AEventVisitor::ScopedEvent(ctx.useEventVisitor(), ASW("ATemplateNodeHandler_SESSION",28), m_BlockData, AEventVisitor::EL_DEBUG));
  }

  if (m_BlockData.isEmpty())
    return;

  //a_Context
  AOSContext *pContext = ctx.useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  if (!pContext)
  {
    ARope rope("Must have AOSContext object named: ",35);
    rope.append(AOSContext::OBJECTNAME);
    ATHROW_EX(this, AException::NotFound, rope);
  }

  const AXmlElement *pElement = pContext->useSessionData().getData().findElement(m_BlockData);
  if (pElement)
  {
    //a_Found object
    pElement->emitContent(output);
  }
  else
  {
    if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_WARN))
    {
      ARope rope("Unable to find SESSION element: ",32);
      rope.append(m_BlockData);
      pContext->useEventVisitor().startEvent(rope, AEventVisitor::EL_WARN);
    }
  }
}
