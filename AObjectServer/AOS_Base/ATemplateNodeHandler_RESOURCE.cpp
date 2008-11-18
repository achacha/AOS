/*
Written by Alex Chachanashvili

$Id: ATemplateNodeHandler_RESOURCE.cpp 253 2008-08-05 22:53:39Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "ATemplateNodeHandler_RESOURCE.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "AOSContext.hpp"
#include "AEventVisitor.hpp"
#include "AOSServices.hpp"

const AString ATemplateNodeHandler_RESOURCE::TAGNAME("RESOURCE",8);

void ATemplateNodeHandler_RESOURCE::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateNodeHandler_RESOURCE::ATemplateNodeHandler_RESOURCE()
{
}

ATemplateNodeHandler_RESOURCE::~ATemplateNodeHandler_RESOURCE()
{
}

void ATemplateNodeHandler_RESOURCE::init()
{
}

void ATemplateNodeHandler_RESOURCE::deinit()
{
}

const AString& ATemplateNodeHandler_RESOURCE::getTagName() const
{
  return ATemplateNodeHandler_RESOURCE::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_RESOURCE::create(AFile& file)
{
  ATemplateNodeHandler_RESOURCE::Node *pNode = new ATemplateNodeHandler_RESOURCE::Node(this);
  pNode->fromAFile(file);
  return pNode;
}

///////////////////////////////////////// ATemplateNodeHandler_RESOURCE::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_RESOURCE::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_RESOURCE::Node::Node(const ATemplateNodeHandler_RESOURCE::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_RESOURCE::Node::~Node()
{
}

void ATemplateNodeHandler_RESOURCE::Node::process(ATemplateContext& ctx, AOutputBuffer& output)
{
  AAutoPtr<AEventVisitor::ScopedEvent> scoped;
  if (ctx.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
  {
    scoped.reset(new AEventVisitor::ScopedEvent(ctx.useEventVisitor(), ASW("ATemplateNodeHandler_RESOURCE",28), m_BlockData, AEventVisitor::EL_DEBUG));
  }

  // Get resource
  AOSContext *pContext = ctx.useObjects().useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
  if (!pContext)
  {
    ARope rope("Must have AOSContext object named: ",35);
    rope.append(AOSContext::OBJECTNAME);
    ATHROW_EX(this, AException::NotFound, rope);
  }

  LIST_AString languages;
  pContext->useRequestHeader().getAcceptLanguageList(languages);
  const AXmlDocument *pDoc = pContext->useServices().useResourceManager().getResources(languages);
  if (!pDoc)
  {
    if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_WARN))
    {
      pContext->useEventVisitor().startEvent(ASW("Unable to find resource container for locales in Accept-Languages",65), AEventVisitor::EL_WARN);
    }
    return;
  }
  
  const AXmlElement *pElement = pDoc->getRoot().findElement(m_BlockData);
  if (pElement)
  {
    //a_Found object
    pElement->emitContent(output);
  }
  else
  {
    if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_WARN))
    {
      ARope rope("Unable to find resource element: ",33);
      rope.append(m_BlockData);
      pContext->useEventVisitor().startEvent(rope, AEventVisitor::EL_WARN);
    }
  }
}
