/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AXmlDocument.hpp"
#include "AFile.hpp"
#include "AString.hpp"
#include "AXmlInstruction.hpp"
#include "AException.hpp"

const AString AXmlDocument::DEFAULT_ROOT("root",4);

void AXmlDocument::debugDump(std::ostream& os /* = std::cerr */, int indent /* = 0x0 */) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Instructions={" << std::endl;
  LIST_NODEPTR::const_iterator cit = m_Instructions.begin();
  while (cit != m_Instructions.end())
  {
    (*cit)->debugDump(os, indent+1);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Root={" << std::endl;
  m_Root.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}


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

AXmlDocument::AXmlDocument(const AXmlDocument& that) :
  m_Root(that.m_Root)
{
  for (LIST_NODEPTR::const_iterator cit = that.m_Instructions.begin(); cit != that.m_Instructions.end(); ++cit)
  {
    AXmlElement *pClone = (*cit)->clone();
    m_Instructions.push_back(dynamic_cast<AXmlInstruction *>(pClone));
  }
}

AXmlDocument::~AXmlDocument()
{
  try
  {
    for(LIST_NODEPTR::iterator it = m_Instructions.begin(); it != m_Instructions.end(); ++it)
      delete (*it);
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
    for (int i=0; i<indent; ++i) target.append(AConstant::ASTRING_TWOSPACES);    //a_Indentation
    (*cit)->emit(target, indent);
    target.append(AConstant::ASTRING_CRLF);
    ++cit;
  }
  m_Root.emit(target, indent);
}

void AXmlDocument::emitJson(
  AOutputBuffer& target, 
  int indent // = -1
) const
{
  if (m_Instructions.size() > 0)
  {
    for (int i=0; i<indent; ++i) target.append(AConstant::ASTRING_TWOSPACES);
    target.append('{');
    if (indent >=0) target.append(AConstant::ASTRING_CRLF);

    LIST_NODEPTR::const_iterator cit = m_Instructions.begin();
    while (cit != m_Instructions.end())
    {
      (*cit)->emitJson(target, (indent >= 0 ? indent+1 : indent));
      ++cit;
    }
    m_Root.emitJson(target, (indent >= 0 ? indent+1 : indent));
    if (indent >= 0) target.append(AConstant::ASTRING_CRLF);
    for (int i=0; i<indent; ++i) target.append(AConstant::ASTRING_TWOSPACES);
    target.append('}');
  }
  else
  {
    //a_Each node will emit {} around 'object'
    m_Root.emitJson(target, indent);
  }
}

void AXmlDocument::clear(
  const AString& rootName, //  = AConstant::ASTRING_EMPTY
  bool keepInstructions    //  = false
)
{
  // Clear the DOM
  m_Root.clear();

  // Clear instructions
  if (!keepInstructions)
  {
    LIST_NODEPTR::iterator it = m_Instructions.begin();
    while (it != m_Instructions.end())
    {
      delete (*it);
      ++it;
    }
    m_Instructions.clear();

    //a_Madantory first instruction:  <?xml version="1.0" encoding="UTF-8"?>
    AXmlInstruction *p = new AXmlInstruction(AXmlInstruction::XML_HEADER);
    p->addAttribute(ASW("version", 7), ASW("1.0", 3));
    p->addAttribute(ASW("encoding", 8), ASW("UTF-8", 5));
    m_Instructions.push_back(p);
  }

  // Set name if specified
  if (!rootName.isEmpty())
    m_Root.useName().assign(rootName);
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
  //a_Full clear since we are restoring from file
  clear(AConstant::ASTRING_EMPTY, false);
  
  AString str(256, 128);
  char c;

  bool doLoop = true;
  while (doLoop)
  {
    //a_Find < or break if EOF
    if (AConstant::npos == file.skipUntilOneOf('<'))
      break;

    if (AConstant::npos == file.skipUntilNotOneOf())                  //a_Skip over whitespace between < and tagname
      break;

    //a_Determine if it's an instruction or element
    file.peek(c);
    if (c == '?' || c == '!')
    {
      file.read(str, 1);

      //a_Extract name and skip over whitespace
      if (AConstant::npos == file.readUntilOneOf(str, AXmlElement::sstr_EndInstructionOrWhitespace, false))
        ATHROW(this, AException::InvalidData);

      if (0 != str.find(ASW("!--",3)))
      {
        //a_Comment not found
        if (AConstant::npos == file.skipUntilNotOneOf())
          ATHROW(this, AException::InvalidData);
      }
        
      //a_Xml instruction (singular by default)
      AAutoPtr<AXmlInstruction> p(new AXmlInstruction(str), true);
      
      str.clear();
      if (AConstant::npos == file.readUntil(str, p->getTypeTerminator()))
        ATHROW_EX(&file, AException::InvalidData, AString("Cannot find terminator for type: ")+p->getTypeTerminator());

      if (AConstant::npos == file.skipUntilNotOneOf())
        ATHROW(&file, AException::InvalidData);

      p->parse(str);

      m_Instructions.push_back(p);
      p.setOwnership(false);
    }
    else
    {
      file.putBack('<');     
      m_Root.fromAFile(file);
    }
    
    str.clear();
  }
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

AXmlElement& AXmlDocument::emitXml(AXmlElement& thisRoot) const
{
  return m_Root.emitXml(thisRoot);
}

AXmlDocument* AXmlDocument::clone() const
{ 
  return new AXmlDocument(*this);
}
