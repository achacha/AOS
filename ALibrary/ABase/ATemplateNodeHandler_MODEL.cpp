#include "pchABase.hpp"
#include "ATemplateNodeHandler_MODEL.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "ABasePtrContainer.hpp"

const AString ATemplateNodeHandler_MODEL::TAGNAME("MODEL",5);

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

ATemplateNode *ATemplateNodeHandler_MODEL::create(AFile& file)
{
  ATemplateNodeHandler_MODEL::Node *pNode = new ATemplateNodeHandler_MODEL::Node(this);
  pNode->fromAFile(file);
  return pNode;
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

void ATemplateNodeHandler_MODEL::Node::process(ABasePtrContainer& objects, AOutputBuffer& output)
{
  //a_If this assert failed, theis object was not constructed correctly
  AXmlDocument *pDoc = objects.useAsPtr<AXmlDocument>(ATemplate::OBJECTNAME_MODEL);
  if (!pDoc)
    ATHROW_EX(this, AException::NotFound, ARope("Must have AXmlDocument model object named: ")+ATemplate::OBJECTNAME_MODEL);

  AXmlElement *pElement = pDoc->useRoot().findElement(m_BlockData);
  if (pElement)
  {
    //a_Found object
    pElement->emitContent(output);
  }
  else
  {
    output.append(ARope("Unable to find element for '")+m_BlockData+AConstant::ASTRING_SINGLEQUOTE);
  }
}

void ATemplateNodeHandler_MODEL::Node::_handleDataAfterRead()
{
  m_BlockData.stripLeadingAndTrailing();
}
