#include "pchABase.hpp"
#include "ATemplateNode_Code.hpp"
#include "AObjectContainer.hpp"
#include "ATemplate.hpp"
#include "AXmlElement.hpp"

const AString ATemplateNode_Code::TAGNAME("CODE",4);
const AString ATemplateNode_Code::sstr_LineDelimeter(";");

#ifdef __DEBUG_DUMP__
void ATemplateNode_Code::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ATemplateNode_Code @ " << std::hex << this << std::dec << ") {" << std::endl;
  LIST_AString::const_iterator cit = m_Lines.begin();
  while (cit != m_Lines.end())
  {
    ADebugDumpable::indent(os, indent+1) << *cit << ";" << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ATemplateNode_Code::ATemplateNode_Code(ATemplate& t) :
  ATemplateNode(t)
{
}

ATemplateNode_Code::ATemplateNode_Code(const ATemplateNode_Code& that) :
  ATemplateNode(that),
  m_Lines(that.m_Lines)
{
}

const AString& ATemplateNode_Code::getTagName() const
{
  return TAGNAME;
}

ATemplateNode* ATemplateNode_Code::create(ATemplate& t, AFile& file)
{
  ATemplateNode_Code *p = new ATemplateNode_Code(t);
  p->fromAFile(file);
  return p;
}

void ATemplateNode_Code::emitXml(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("ATemplateNode_Code",18));

  LIST_AString::const_iterator cit = m_Lines.begin();
  while (cit != m_Lines.end())
  {
    target.addElement(ASW("line",4), *cit);
    ++cit;
  }
}

void ATemplateNode_Code::emit(AOutputBuffer& target) const
{
  target.append(ATemplate::TAG_START);
  target.append(TAGNAME);
  target.append(ATemplate::BLOCK_START);
  target.append(AConstant::ASTRING_CRLF);

  LIST_AString::const_iterator cit = m_Lines.begin();
  while (cit != m_Lines.end())
  {
    target.append(*cit);
    target.append(';');
    target.append(AConstant::ASTRING_CRLF);
    ++cit;
  }

  target.append(ATemplate::BLOCK_END);
  target.append(TAGNAME);
  target.append(ATemplate::TAG_END);
}

void ATemplateNode_Code::toAFile(AFile& aFile) const
{
  aFile.write(ATemplate::TAG_START);
  aFile.write(TAGNAME);
  aFile.writeLine(ATemplate::BLOCK_START);

  LIST_AString::const_iterator cit = m_Lines.begin();
  while (cit != m_Lines.end())
  {
    aFile.write(*cit);
    aFile.writeLine(ATemplateNode_Code::sstr_LineDelimeter);
    ++cit;
  }

  aFile.write(ATemplate::BLOCK_END);
  aFile.write(TAGNAME);
  aFile.write(ATemplate::TAG_END);
}

void ATemplateNode_Code::fromAFile(AFile& aFile)
{
  AFile_AString strfile(2048, 2048);

  //a_End tag delimiter
  AString endToken(ATemplate::BLOCK_END);
  endToken.append(TAGNAME);
  endToken.append(ATemplate::TAG_END);
  
  //a_Read until end token into a string file which is parsed on per-line basis
  if (AConstant::npos != aFile.readUntil(strfile.useAString(), endToken, true, true))
  {
    AString strLine(512,512);
    while (AConstant::npos != strfile.readUntil(strLine, ATemplateNode_Code::sstr_LineDelimeter, true, true))
    {
      strLine.stripEntire();
      if (!strLine.isEmpty())
        m_Lines.push_back(strLine);
      strLine.clear();
    }
    strfile.readUntilEOF(strLine);
    strLine.stripEntire();
    if (!strLine.isEmpty())
      m_Lines.push_back(strLine);
  }
}

bool ATemplateNode_Code::isEmpty() const
{
  return (m_Lines.size() == 0);
}

void ATemplateNode_Code::process()
{
  LIST_AString::const_iterator cit = m_Lines.begin();
  while (cit != m_Lines.end())
  {
    _processLine(*cit);
    ++cit;
  }
}

void ATemplateNode_Code::_processLine(const AString& line)
{
  AOutputBuffer& output = m_ParentTemplate.useOutput();
  AXmlElement& root = m_ParentTemplate.useModel().useRoot();
  
  //a_First find '(' and ')'
  size_t start = line.find('(');
  size_t end = line.rfind(')');

  if (AConstant::npos == start || AConstant::npos == end || end < start)
    ATHROW(this, AException::InvalidObject);

  //a_Extract command and parameter
  AString command, param;
  line.peek(command, 0, start);
  line.peek(param, start+1, end-start-1);
  param.stripEntire();

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
