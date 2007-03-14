#include "pchABase.hpp"

#include "AXmlDocument.hpp"
#include "AFile.hpp"
#include "AString.hpp"
#include "AXmlInstruction.hpp"
#include "AXmlData.hpp"
#include "ARope.hpp"
#include "AException.hpp"

const AString AXmlDocument::sstr_Start("<");
const AString AXmlDocument::sstr_StartComment("<!--");
const AString AXmlDocument::sstr_StartInstruction("<?");
const AString AXmlDocument::sstr_StartEnd("</");
const AString AXmlDocument::sstr_EndSingular("/>");
const AString AXmlDocument::sstr_End(">");
const AString AXmlDocument::sstr_EndOrWhitespace("/> \t\n\r");
const AString AXmlDocument::sstr_EndComment("-->");
const AString AXmlDocument::sstr_EndInstruction("?>");
const AString AXmlDocument::sstr_EndInstructionOrWhitespace("?> \t\n\r");

#ifdef __DEBUG_DUMP__
void AXmlDocument::debugDump(std::ostream& os /* = std::cerr */, int indent /* = 0x0 */) const
{
  ADebugDumpable::indent(os, indent) << "(AXmlDocument @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Instructions={" << std::endl;
  LIST_NODEPTR::const_iterator cit = m_Instructions.begin();
  while (cit != m_Instructions.end())
  {
    (*cit)->debugDump(os, indent+1);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Root=";
  m_Root.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif


AXmlDocument::AXmlDocument(
  const AString& rootName,
  AXmlInstruction *pXmlHeader // = NULL
) :
  m_Root(rootName)
{
  if (!pXmlHeader)
  {
    //a_Madantory first instruction:  <?xml version="1.0" encoding="UTF-8"?>
    AXmlInstruction *p = new AXmlInstruction(AXmlInstruction::XML_HEADER);
    p->addAttribute(ASW("version", 7), ASW("1.0", 3));
    p->addAttribute(ASW("encoding", 8), ASW("UTF-8", 5));
    m_Instructions.push_back(p);
  }
  else
  {
    if (AXmlInstruction::XML_HEADER == pXmlHeader->getType())
      m_Instructions.push_back(pXmlHeader);
    else
      ATHROW(this, AException::InvalidParameter);
  }
}

AXmlDocument::AXmlDocument(AFile& file)
{
  fromAFile(file);
}

AXmlDocument::~AXmlDocument()
{
  try
  {
    LIST_NODEPTR::iterator it = m_Instructions.begin();
    while (it != m_Instructions.end())
    {
      delete (*it);
      ++it;
    }
  }
  catch(...) {}
}

void AXmlDocument::emit(AOutputBuffer& target) const
{
  emit(target, -1);
}

void AXmlDocument::emit(AOutputBuffer& target, int indent) const
{
  LIST_NODEPTR::const_iterator cit = m_Instructions.begin();
  while (cit != m_Instructions.end())
  {
    (*cit)->emit(target, indent);
    ++cit;
  }
  m_Root.emit(target, indent);
}

void AXmlDocument::emitJSON(
  AOutputBuffer& target, 
  int indent // = -1
) const
{
  LIST_NODEPTR::const_iterator cit = m_Instructions.begin();
  while (cit != m_Instructions.end())
  {
    (*cit)->emitJSON(target, indent);
    if (indent >= 0)
      target.append(AString::sstr_CRLF);

    ++cit;
  }
  m_Root.emitJSON(target, indent);
}

void AXmlDocument::clearAll()
{
  LIST_NODEPTR::iterator it = m_Instructions.begin();
  while (it != m_Instructions.end())
  {
    delete (*it);
    ++it;
  }
  m_Instructions.clear();
  m_Root.clear();
  m_Root.useName().clear();
}

void AXmlDocument::clear()
{
  m_Root.clear();
}

AXmlInstruction& AXmlDocument::addInstruction(AXmlInstruction::TYPE type)
{
  AXmlInstruction *p = new AXmlInstruction(type);
  m_Instructions.push_back(p);
  return *p;
}

AXmlInstruction& AXmlDocument::addComment(const AString& comment)
{
  AXmlInstruction *p = new AXmlInstruction(AXmlInstruction::COMMENT);
  p->useData().set(comment);
  m_Instructions.push_back(p);
  return *p;
}

void AXmlDocument::fromAFile(AFile& file)
{
  AString str(256, 128);
  char c;

  while (true)
  {
    //a_Find < or break if EOF
    if (AConstant::npos == file.skipUntilOneOf('<'))
      break;

    if (AConstant::npos == file.skipOver())                  //a_Skip over whitespace between < and tagname
      break;

    //a_Determine if it's an instruction or element
    file.peek(c);
    if (c == '?' || c == '!')
    {
      file.read(str, 1);

      //a_Extract name and skip over whitespace
      if (AConstant::npos == file.readUntilOneOf(str, sstr_EndInstructionOrWhitespace, false))
        ATHROW(this, AException::InvalidData);

      if (AConstant::npos == file.skipOver())
        ATHROW(this, AException::InvalidData);
      
      //a_Xml instruction (singular by default)
      AAutoPtr<AXmlInstruction> p(new AXmlInstruction(str));
      
      str.clear();
      if (AConstant::npos == file.readUntil(str, p->getTypeTerminator()))
        ATHROW_EX(&file, AException::InvalidData, (AString("Cannot find terminator for type: ")+p->getTypeTerminator()).c_str());

      if (AConstant::npos == file.skipOver())
        ATHROW(&file, AException::InvalidData);

      p->parse(str);

      m_Instructions.push_back(p);
      p.setOwnership(false);
    }
    else
    {
      file.putBack('<');     
      _parseElement(m_Root, file);
    }
    
    str.clear();
  }
}

bool AXmlDocument::_parseElement(AXmlElement& parent, AFile& file)
{
  AString str(256, 128);
  char c = ' ';

  //a_Find <
  if (AConstant::npos == file.skipUntilOneOf('<'))
    return false;

  file.skipOver();                  //a_Skip over whitespace between < and tagname

  //a_Extract name and skip over whitespace
  file.readUntilOneOf(str, sstr_EndOrWhitespace, false);
  file.skipOver();
  parent.useName().assign(str);
  
  //a_Find /> or >
  str.clear();
  file.readUntilOneOf(str, sstr_End);
  if (!str.isEmpty() && str.at(str.getSize() - 1) == '/')
  {
    c = '/';  //a_Singular mode
    str.setSize(str.getSize() - 1);
  }
  str.stripTrailing();

  //a_Parse attributes
  if (!str.isEmpty())
    parent.useAttributes().parse(str);

  bool boolEndTagFound = (c == '/');
  while (!boolEndTagFound)
  {
    //a_Read data until next tag starts
    if (AConstant::npos == file.skipOver())                //a_Skip over whitespace
      ATHROW(this, AException::InvalidData);

    str.clear();
    if (AConstant::npos == file.readUntilOneOf(str, sstr_Start))
      ATHROW_EX(this, AException::InvalidData, AString("Inside element: ")+parent.getName());

    str.stripTrailing();
    if (!str.isEmpty())
    {
      parent.addData(str);
    }

    //a_Skip over whitespace between < and start of the tag name
    if (AConstant::npos == file.skipOver())
      ATHROW_EX(this, AException::InvalidData, AString("Inside element: ")+parent.getName());

    file.peek(c);
    if (c == '/')
    {
      //a_End of tag found
      file.read(c);
      str.clear();
      if (AConstant::npos == file.readUntilOneOf(str, sstr_EndOrWhitespace))
        ATHROW_EX(this, AException::InvalidData, AString("Inside element: ")+parent.getName());
      file.skipOver(sstr_EndOrWhitespace);                //a_Skip over whitespace and >
      if (str != parent.getName())
        ATHROW_EX(this, AException::InvalidData, AString("Close tag </")+str+"> does not match opened tag <"+parent.getName()+">");
      
      boolEndTagFound = true;
    }
    else
    {
      //a_Put the start tag back
      file.putBack('<');

      //a_Another element
      AAutoPtr<AXmlElement> psubElement(new AXmlElement(&parent));
      psubElement->fromAFile(file);
      psubElement.setOwnership(false);
      parent.addContent(psubElement.get());
    }
  }

  return true;
}

void AXmlDocument::toAFile(AFile& afile) const
{
  LIST_NODEPTR::const_iterator cit = m_Instructions.begin();
  while (cit != m_Instructions.end())
  {
    (*cit)->emit(afile);
    ++cit;
  }
  m_Root.toAFile(afile);
}

AXmlElement& AXmlDocument::useRoot()
{
  return m_Root;
}

const AXmlElement& AXmlDocument::getRoot() const
{
  return m_Root;
}
