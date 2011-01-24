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

void ATemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
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
)
{
  _loadDefaultHandlers(mask);

  AFile_AString strfile(source);
  fromAFile(strfile);

  _initializeHandlers();
}

ATemplate::ATemplate(
  AFile& source, 
  HandlerMask mask // = ATemplate::HANDLER_ALL
)
{
  _loadDefaultHandlers(mask);

  fromAFile(source);

  _initializeHandlers();
}

ATemplate::ATemplate(
  HandlerMask mask // = ATemplate::HANDLER_ALL
)
{
  _loadDefaultHandlers(mask);
  _initializeHandlers();
}

ATemplate::~ATemplate()
{
  try
  {
    //a_Delete nodes
    for (NODES::iterator it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
      delete (*it);
  
    _deinitializeHandlers();
  }
  catch(...) {}
}

void ATemplate::_initializeHandlers()
{
  //a_Initialize handlers
  for (HANDLERS::iterator it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
    (*it).second->init();
}

void ATemplate::_deinitializeHandlers()
{
  //a_Delete handlers
  for (HANDLERS::iterator it = m_Handlers.begin(); it != m_Handlers.end(); ++it)
  {
    it->second->deinit();
    delete it->second;
  }
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
  AAutoPtr<ATemplateNode> pNode(pHandler->create(&source), true);
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

void ATemplate::fromAString(const AString& source)
{
  AFile_AString strfile(source);
  fromAFile(strfile);
}

void ATemplate::fromAFile(AFile& aFile)
{
  AString str(1024, 256);
  AString temp(1024, 1024);
  AAutoPtr<ATemplateNode> pText(new ATemplateNode(), true);  //a_Text block accumulator

  // Find start, everything until that is a text block
  AString tagName;
  AString attributeBlock;
  char delimiter;
  size_t ret = AConstant::npos;
  while(AConstant::npos != (ret = aFile.readUntil(pText->useBlockData(), AXmlElement::sstr_Start)))
  {
    temp.clear();
    aFile.readUntilNotOneOf(temp);

    aFile.readUntilOneOf(tagName, AXmlElement::sstr_End, true, false);  // remove but keep delimiter
    delimiter = tagName.rget();
    temp.append(tagName);
    
    bool isSingular = false;
    if ('/' == tagName.last())
    {
      // Singular form
      tagName.rremove();
      isSingular = true;
    }
    tagName.stripTrailing();

    //a_Extract attributes
    size_t endTag = tagName.find(' ');
    if (AConstant::npos != endTag)
    {
      //a_Possible attributes
      tagName.get(attributeBlock, endTag);
    }

    // Check if tag is registered
    HANDLERS::iterator it = m_Handlers.find(tagName);
    if (m_Handlers.end() == it)
    {
      //a_No such tag, add to the current text node and keep going
      pText->useBlockData().append(AXmlElement::sstr_Start);
      pText->useBlockData().append(tagName);
      
      //a_Put back and reparse in case attributes contain tags
      aFile.putBack(delimiter);
      aFile.putBack(attributeBlock);
    }
    else
    {
      //a_Add text first if not empty
      if (!pText->useBlockData().isEmpty())
      {
        m_Nodes.push_back(pText.use());
        pText.setOwnership(false);

        //a_Create new active text node
        pText.reset(new ATemplateNode(), true);
      }

      //a_Handle the node
      ATemplateNode *pNode = NULL;
      if (isSingular)
        pNode = it->second->create();   //a_Singular nodes do not need parsing
      else
        pNode = it->second->create(&aFile);

      if (!attributeBlock.isEmpty())
      {
        pNode->useAttributes().parse(attributeBlock);
      }

      //a_Add parsed node
      m_Nodes.push_back(pNode);
    }
    tagName.clear();
    attributeBlock.clear();
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
  AOutputBuffer& output
)
{
  if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
  {
    context.useEventVisitor().startEvent(ASW("ATemplate::process",18), AEventVisitor::EL_INFO);
  }
  
  for (NODES::const_iterator cit = m_Nodes.begin(); cit != m_Nodes.end(); ++cit)
  {
    (*cit)->process(context, output);
  }

  if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
  {
    context.useEventVisitor().endEvent();
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
