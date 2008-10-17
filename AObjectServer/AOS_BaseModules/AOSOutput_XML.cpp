/*
Written by Alex Chachanashvili

$Id: AOSOutput_XML.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_XML.hpp"
#include "AXmlDocument.hpp"

const AString& AOSOutput_XML::getClass() const
{
  static const AString CLASS("XML");
  return CLASS;
}

AOSOutput_XML::AOSOutput_XML(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_XML::execute(AOSContext& context)
{
  AXmlElement::CONST_CONTAINER paths;
  AXmlElement::CONST_CONTAINER nodes;
  if (context.getOutputParams().find(AOS_BaseModules_Constants::PATH, paths))
  {
    // Get the path fragments and convert into nodes
    for (AXmlElement::CONST_CONTAINER::const_iterator cit = paths.begin(); cit != paths.end(); ++cit)
    {
      // Get path and find associated nodes
      AString path;
      (*cit)->emitContent(path);
      context.useModel().find(path, nodes);
    }
  }

  AString clientXslStylesheet;
  context.getOutputParams().emitContentFromPath("clientside-xsl-stylesheet", clientXslStylesheet);

  AXmlDocument xmlDoc(AOSContext::XML_ROOT);
  if (nodes.size() > 0)
  {
    for (AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit)
    {
      if (context.useEventVisitor().isLoggingDebug())
      {
        AString str;
        str.append("Emitting the XML from path: ");
        (*cit)->emitPath(str);
        context.useEventVisitor().addEvent(str, AEventVisitor::EL_DEBUG);
      }
      
      xmlDoc.useRoot().addContent(*(*cit));
    }
    
    // Add client side stylesheet if specified
    if (!clientXslStylesheet.isEmpty())
    {
      xmlDoc.addInstruction(AXmlInstruction::XML_STYLESHEET)
        .addAttribute(ASW("type",4), ASW("text/xsl",8))
        .addAttribute(ASW("href",4), clientXslStylesheet);
    }
    xmlDoc.emit(context.useOutputBuffer());
  }
  else
  {
    // Entire context model to JSON
    if (context.useEventVisitor().isLoggingDebug())
    {
      context.useEventVisitor().addEvent(ASWNL("Emitting the entire model as XML object"), AEventVisitor::EL_DEBUG);
    }
    
    // Add client side stylesheet if specified
    if (!clientXslStylesheet.isEmpty())
    {
      context.useModelXmlDocument().addInstruction(AXmlInstruction::XML_STYLESHEET)
        .addAttribute(ASW("type",4), ASW("text/xsl",8))
        .addAttribute(ASW("href",4), clientXslStylesheet);
    }
    context.useModelXmlDocument().emit(context.useOutputBuffer());
  }
  
  // Set the correct content type for XML extension
  m_Services.useConfiguration().setMimeTypeFromExt(ASW("xml",3), context);

  return AOSContext::RETURN_OK;
}
