/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATemplate.hpp"
#include "ABasePtrContainer.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "ATemplateNodeHandler_OBJECT.hpp"
#include "ATemplateNodeHandler_MODEL.hpp"

const AString ATemplate::TAG_START("%[",2);
const AString ATemplate::BLOCK_START("]{{{",4);
const AString ATemplate::BLOCK_END("}}}[",4);
const AString ATemplate::TAG_END("]%",2);

void ATemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Initialized=" << AString::fromBool(m_Initialized) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Nodes={  ";
  ADebugDumpable::indent(os, indent+2) << "size=" << m_Nodes.size() << std::endl;
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplate::ATemplate(
  const AString& source,
  HandlerMask mask // = ATemplate::HANDLER_ALL
) :
  m_Initialized(false)
{
  _loadDefaultHandlers(mask);

  AFile_AString strfile(source);
  fromAFile(strfile);
}

ATemplate::ATemplate(
  AFile& source, 
  HandlerMask mask // = ATemplate::HANDLER_ALL
) :
  m_Initialized(false)
{
  _loadDefaultHandlers(mask);

  fromAFile(source);
}

ATemplate::ATemplate(
  HandlerMask mask // = ATemplate::HANDLER_ALL
) :
  m_Initialized(false)
{
  _loadDefaultHandlers(mask);
}

ATemplate::~ATemplate()
{
  try
  {
    //a_Delete nodes
    for (NODES::iterator it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
      delete (*it);
  
    //a_Delete handlers
    for (HANDLERS::iterator it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
      delete it->second;
  }
  catch(...) {}
}

void ATemplate::_loadDefaultHandlers(ATemplate::HandlerMask mask)
{
  if (mask & ATemplate::HANDLER_OBJECT)
  {
    addHandler(new ATemplateNodeHandler_OBJECT());
  }
  if (mask & ATemplate::HANDLER_MODEL)
  {
    addHandler(new ATemplateNodeHandler_MODEL());
  }
}

void ATemplate::addNode(const AString& tagname, AFile& source)
{
  AASSERT_EX(this, m_Handlers.end() != m_Handlers.find(tagname), tagname);  //a_Tag handler not found
  ATemplateNodeHandler *pHandler = m_Handlers[tagname];
  AAutoPtr<ATemplateNode> pNode(pHandler->create(source), true);
  m_Nodes.push_back(pNode);
  pNode.setOwnership(false);
}

void ATemplate::addHandler(ATemplateNodeHandler *pHandler)
{
  AASSERT_EX(this, m_Handlers.end() == m_Handlers.find(pHandler->getTagName()), pHandler->getTagName());  //a_Duplicate tag handler detected
  m_Handlers[pHandler->getTagName()] = pHandler;
}

void ATemplate::toAFile(AFile& aFile) const
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->toAFile(aFile);
    ++cit;
  }
}

void ATemplate::fromAFile(AFile& aFile)
{
  AString tagName;
  AAutoPtr<ATemplateNode> pText(new ATemplateNode(), true);  //a_Text block accumulator
  size_t ret = AConstant::npos;
  while(AConstant::npos != (ret = aFile.readUntil(pText->useBlockData(), ATemplate::TAG_START)))
  {
    if (AConstant::npos == (ret = aFile.readUntil(tagName, ATemplate::BLOCK_START)))
    {
      pText->useBlockData().append(ATemplate::TAG_START);
      break;  //a_No more
    }

    //a_Get the handler 
    HANDLERS::iterator it = m_Handlers.find(tagName);
    if (m_Handlers.end() == it)
    {
      //a_No such tag, add to the current text node and keep going
      pText->useBlockData().append(ATemplate::TAG_START);
      pText->useBlockData().append(tagName);
      pText->useBlockData().append(ATemplate::BLOCK_START);
    }
    else
    {
      //a_Add text first if not empty
      if (!pText->useBlockData().isEmpty())
      {
        m_Nodes.push_back(pText.use());
        pText.setOwnership(false);

        //a_Create new active text node
        pText.reset(new ATemplateNode());
      }

      //a_Handle the node
      ATemplateNode *pNode = it->second->create(aFile);

      //a_Add parsed node
      m_Nodes.push_back(pNode);
    }
    tagName.clear();
  }

  //a_Read to EOF
  aFile.readUntilEOF(pText->useBlockData());

  //a_Leftover text added if not empty
  if (!pText->useBlockData().isEmpty())
  {
    m_Nodes.push_back(pText.use());
    pText.setOwnership(false);
  }
}

void ATemplate::process(
  ATemplateContext& context,
  AOutputBuffer& output,
  bool hibernateHandlersWhenDone  // = false
)
{
  if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
  {
    context.useEventVisitor().startEvent(ASW("ATemplate::process",18), AEventVisitor::EL_INFO);
  }
  
  if (!m_Initialized)
  {
    //a_Initialize handlers
    for (HANDLERS::iterator it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
      (*it).second->init();

    m_Initialized = true;
  }

  if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
  {
    context.useEventVisitor().startEvent(ASW("ATemplate::process:loop",23), AEventVisitor::EL_DEBUG);
  }

  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->process(context, output);
    ++cit;
  }

  if (hibernateHandlersWhenDone)
  {
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
    {
      context.useEventVisitor().startEvent(ASW("ATemplate::process:hibernate",28), AEventVisitor::EL_DEBUG);
    }
    hibernate();
  }

  if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
  {
    context.useEventVisitor().endEvent();
  }
}

void ATemplate::hibernate()
{
  if (m_Initialized)
  {
    //a_De-initialize handlers
    for (HANDLERS::iterator it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
      (*it).second->deinit();

    m_Initialized = false;
  }
}

AXmlElement& ATemplate::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  for (NODES::const_iterator cit = m_Nodes.begin(); cit != m_Nodes.end(); ++cit)
    (*cit)->emitXml(thisRoot.addElement(ASW("node",4)));

  return thisRoot;
}

void ATemplate::emit(AOutputBuffer& target) const
{
  NODES::const_iterator cit = m_Nodes.begin();
  while (cit != m_Nodes.end())
  {
    (*cit)->emit(target);
    ++cit;
  }
}

void ATemplate::clear()
{
  //a_Delete nodes
  for (NODES::iterator it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
    delete (*it);
}
