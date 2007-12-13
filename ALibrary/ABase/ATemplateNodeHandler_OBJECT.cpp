#include "pchABase.hpp"
#include "ATemplateNodeHandler_OBJECT.hpp"
#include "ATemplate.hpp"
#include "ABasePtrHolder.hpp"

const AString ATemplateNodeHandler_OBJECT::TAGNAME("OBJECT",6);

#ifdef __DEBUG_DUMP__
void ATemplateNodeHandler_OBJECT::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

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

void ATemplateNodeHandler_OBJECT::Node::process(ABasePtrHolder& objects, AOutputBuffer& output)
{
  //a_If this assert failed, theis object was not constructed correctly
//  AXmlDocument *pDoc = objects.useAsPtr<AXmlDocument>(ATemplate::OBJECTNAME_MODEL);
//  if (!pDoc)
//    ATHROW_EX(this, AException::NotFound, ARope("Must have AXmlDocument model object named: ")+ATemplate::OBJECTNAME_MODEL);

  const AEmittable *pObject = objects.getAsPtr<const AEmittable>(m_BlockData);
  if (pObject)
  {
    //a_Found object
    pObject->emit(output);
  }
  else
  {
    output.append(ARope("Unable to find object or not AEmittable: '")+m_BlockData+AConstant::ASTRING_SINGLEQUOTE);
  }
}
