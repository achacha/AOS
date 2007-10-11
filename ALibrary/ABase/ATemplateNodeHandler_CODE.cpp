#include "pchABase.hpp"
#include "ATemplateNodeHandler_CODE.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"
#include "ATemplate.hpp"

const AString ATemplateNodeHandler_CODE::TAGNAME("CODE",4);

#ifdef __DEBUG_DUMP__
void ATemplateNodeHandler_CODE::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNodeHandler_CODE::ATemplateNodeHandler_CODE()
{
}

ATemplateNodeHandler_CODE::~ATemplateNodeHandler_CODE()
{
}

void ATemplateNodeHandler_CODE::init()
{
}

void ATemplateNodeHandler_CODE::deinit()
{
}

const AString& ATemplateNodeHandler_CODE::getTagName() const
{
  return ATemplateNodeHandler_CODE::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_CODE::create(AFile& file)
{
  ATemplateNodeHandler_CODE::Node *pNode = new ATemplateNodeHandler_CODE::Node(this);
  pNode->fromAFile(file);
  return pNode;
}

///////////////////////////////////////// ATemplateNodeHandler_CODE::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_CODE::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_CODE::Node::Node(const ATemplateNodeHandler_CODE::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_CODE::Node::~Node()
{
}

void ATemplateNodeHandler_CODE::Node::process(ABasePtrHolder& objects, AOutputBuffer& output)
{
  //a_If this assert failed, theis object was not constructed correctly
  AXmlDocument *pDoc = objects.useAsPtr<AXmlDocument>(ATemplate::OBJECTNAME_MODEL);
  if (!pDoc)
    ATHROW_EX(this, AException::InvalidObject, ARope("Must have AXmlDocument model object named: ")+ATemplate::OBJECTNAME_MODEL);

  AFile_AString strf(m_BlockData);
  AString strLine(1024, 1024);
  while (AConstant::npos != strf.readLine(strLine))
  {
    if (!strLine.isEmpty())
    {
      _processLine(strLine, pDoc->useRoot(), output);
      strLine.clear();
    }
  }
}

void ATemplateNodeHandler_CODE::Node::_processLine(const AString& line, AXmlElement& root, AOutputBuffer& output)
{
  //a_First find '(' and ')'
  size_t start = line.find('(');
  size_t end = line.rfind(')');

  if (AConstant::npos == start || AConstant::npos == end || end < start)
    ATHROW(this, AException::InvalidObject);

  //a_Extract command and parameter
  AString command, param;
  line.peek(command, 0, start);
  line.peek(param, start+1, end-start-1);
  command.stripLeadingAndTrailing();
  param.stripLeadingAndTrailing();

  if (command.equalsNoCase("print"))
  {
    //a_Check for attribute
    size_t pos = param.rfind('@');
    AString attrName;
    if (AConstant::npos != pos)
    {
      param.peek(attrName, pos+1);
      param.setSize(pos);
    }

    AXmlNode::ConstNodeContainer nodes;
    root.find(param, nodes);

    AXmlNode::ConstNodeContainer::const_iterator cit = nodes.begin();
    while (cit != nodes.end())
    {
      if (cit != nodes.begin())
        output.append(',');
      if (attrName.isEmpty())
        (*cit)->emitContent(output);
      else
        (*cit)->getAttributes().get(attrName, output);
      
      ++cit;
    }
  }
  else if (command.equalsNoCase("count"))
  {
    AXmlNode::ConstNodeContainer nodes;
    root.find(param, nodes);
    output.append(AString::fromSize_t(nodes.size()));
  }
  else
  {
    output.append("ERROR: UNKNOWN(");
    output.append(command);
    output.append(',');
    output.append(param);
    output.append(')');
  }
}
