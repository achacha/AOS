/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_Base.hpp"
#include "ATemplateNodeHandler_SESSION.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "AOSContext.hpp"

const AString ATemplateNodeHandler_SESSION::TAGNAME("SESSION",7);

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

ATemplateNode *ATemplateNodeHandler_SESSION::create(AFile& file)
{
  ATemplateNodeHandler_SESSION::Node *pNode = new ATemplateNodeHandler_SESSION::Node(this);
  pNode->fromAFile(file);
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

void ATemplateNodeHandler_SESSION::Node::process(ABasePtrContainer& objects, AOutputBuffer& output)
{
  //a_Context
  AOSContext *pContext = objects.useAsPtr<AOSContext>(AOSContext::OBJECTNAME);
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
