/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_JSON.hpp"

const AString AOSOutput_JSON::CLASS("JSON");

const AString& AOSOutput_JSON::getClass() const
{
  return CLASS;
}

AOSOutput_JSON::AOSOutput_JSON(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_JSON::execute(AOSContext& context)
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

  if (nodes.size() > 0)
  {
    if (nodes.size() > 1)
    {
      // Array
      context.useOutputBuffer().append("[\r\n",3);
      AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin();
      while(cit != nodes.end())
      {
        (*cit)->emitJson(context.useOutputBuffer(),1); 
        ++cit;
        if (cit != nodes.end())
          context.useOutputBuffer().append("\r\n,\r\n",5);
      }
      context.useOutputBuffer().append("\r\n]",3);
    }
    else
    {
      // Single object
      nodes.front()->emitJson(context.useOutputBuffer(),0);
    }
  }
  else
  {
    // Entire context model to JSON
    if (context.useEventVisitor().isLoggingDebug())
    {
      context.useEventVisitor().addEvent(ASWNL("Emitting the entire model as JSON object"), AEventVisitor::EL_DEBUG);
    }
    context.useModel().emitJson(context.useOutputBuffer(),0);
  }
  
  // Set the correct content type for XML extension
  m_Services.useConfiguration().setMimeTypeFromExt(ASW("json",4), context);

  return AOSContext::RETURN_OK;
}
