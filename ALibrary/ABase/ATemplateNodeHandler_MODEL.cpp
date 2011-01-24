/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATemplateNodeHandler_MODEL.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "ABasePtrContainer.hpp"

const AString ATemplateNodeHandler_MODEL::TAGNAME("aos:model");

void ATemplateNodeHandler_MODEL::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateNodeHandler_MODEL::ATemplateNodeHandler_MODEL()
{
}

ATemplateNodeHandler_MODEL::~ATemplateNodeHandler_MODEL()
{
}

void ATemplateNodeHandler_MODEL::init()
{
}

void ATemplateNodeHandler_MODEL::deinit()
{
}

const AString& ATemplateNodeHandler_MODEL::getTagName() const
{
  return ATemplateNodeHandler_MODEL::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_MODEL::create(AFile *pFile)
{
  ATemplateNodeHandler_MODEL::Node *pNode = new ATemplateNodeHandler_MODEL::Node(this);
  
  if (pFile)
    pNode->fromAFile(*pFile);

  return pNode;
}

ATemplateNodeHandler *ATemplateNodeHandler_MODEL::clone()
{ 
  return new ATemplateNodeHandler_MODEL();
}

///////////////////////////////////////// ATemplateNodeHandler_MODEL::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_MODEL::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_MODEL::Node::Node(const ATemplateNodeHandler_MODEL::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_MODEL::Node::~Node()
{
}

void ATemplateNodeHandler_MODEL::Node::process(ATemplateContext& context, AOutputBuffer& output)
{
  AAutoPtr<AEventVisitor::ScopedEvent> scoped;
  if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
  {
    scoped.reset(new AEventVisitor::ScopedEvent(context.useEventVisitor(), ASW("ATemplateNodeHandler_MODEL",26), m_BlockData, AEventVisitor::EL_DEBUG));
  }

  if (m_BlockData.isEmpty())
    return;

  AXmlElement *pElement = context.useModel().useRoot().findElement(m_BlockData);
  if (pElement)
  {
    //a_Found object
    pElement->emitContent(output);
  }
  else
  {
    ARope rope("<!--Unable to find element for '",32); //-V112
    rope.append(m_BlockData);
    rope.append("'-->",4); //-V112
    output.append(rope);
  }
}

void ATemplateNodeHandler_MODEL::Node::_handleDataAfterRead()
{
  m_BlockData.stripLeadingAndTrailing();
}
