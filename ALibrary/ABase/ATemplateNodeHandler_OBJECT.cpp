/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATemplateNodeHandler_OBJECT.hpp"
#include "ATemplate.hpp"
#include "ABasePtrContainer.hpp"

const AString ATemplateNodeHandler_OBJECT::TAGNAME("OBJECT",6);

void ATemplateNodeHandler_OBJECT::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateNodeHandler_OBJECT::ATemplateNodeHandler_OBJECT()
{
}

ATemplateNodeHandler_OBJECT::~ATemplateNodeHandler_OBJECT()
{
}

void ATemplateNodeHandler_OBJECT::init()
{
}

void ATemplateNodeHandler_OBJECT::deinit()
{
}

const AString& ATemplateNodeHandler_OBJECT::getTagName() const
{
  return ATemplateNodeHandler_OBJECT::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_OBJECT::create(AFile& file)
{
  ATemplateNodeHandler_OBJECT::Node *pNode = new ATemplateNodeHandler_OBJECT::Node(this);
  pNode->fromAFile(file);
  return pNode;
}

///////////////////////////////////////// ATemplateNodeHandler_OBJECT::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_OBJECT::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_OBJECT::Node::Node(const ATemplateNodeHandler_OBJECT::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_OBJECT::Node::~Node()
{
}

void ATemplateNodeHandler_OBJECT::Node::process(ATemplateContext& context, AOutputBuffer& output)
{
  const AEmittable *pObject = context.useObjects().getAsPtr<const AEmittable>(m_BlockData);
  if (pObject)
  {
    //a_Found object
    pObject->emit(output);
  }
  else
  {
    output.append(ARope("Unable to find object or not AEmittable type: '")+m_BlockData+AConstant::ASTRING_SINGLEQUOTE);
  }
}

void ATemplateNodeHandler_OBJECT::Node::_handleDataAfterRead()
{
  m_BlockData.stripLeadingAndTrailing();
}
