/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_Template.hpp"

const AString AOSOutput_Template::CLASS("Template");

const AString& AOSOutput_Template::getClass() const
{
  return CLASS;
}

void AOSOutput_Template::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSOutput_Template @ " << std::hex << this << std::dec << ") {" << std::endl;

  AOSOutputGeneratorInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AOSOutput_Template::adminEmitXml(
  AXmlElement& eBase, 
  const AHTTPRequestHeader& request
)
{
  AOSOutputGeneratorInterface::adminEmitXml(eBase, request);
}

void AOSOutput_Template::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
}

AOSOutput_Template::AOSOutput_Template(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSOutput_Template::~AOSOutput_Template()
{
}

AOSContext::ReturnCode AOSOutput_Template::execute(AOSContext& context)
{
  //a_See if extension for mime type set for the template(s)
  AString ext;
  if (context.getOutputParams().emitContentFromPath(AOS_BaseModules_Constants::MIME_EXTENSION, ext))
  {
    m_Services.useConfiguration().setMimeTypeFromExt(ext, context);
  }
  else
  {
    //a_Set content type based on request URL extension
    m_Services.useConfiguration().setMimeTypeFromExt(context.useRequestUrl().getExtension(), context);
  }
  
  // Create a new template
  AAutoPtr<ATemplate> pTemplate(m_Services.createTemplate(), true);

  // Iterate parameters and build the template
  const AXmlElement::CONTAINER& container = context.getOutputParams().getContentContainer();
  for (AXmlElement::CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
  {
    //a_Check "if" condition
    AString ifElement;
    if ((*cit)->getAttributes().get(ASW("if",2), ifElement))
    {
      if (ifElement.getSize() > 0)
      {
        //a_Check condition, if not met continue with next template
        if (!context.useModel().exists(ifElement))
        {
          if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
            context.useEventVisitor().startEvent(ARope("Skipping conditional file template IF ")+ifElement, AEventVisitor::EL_DEBUG);
          continue;
        }
      }
    }
    //a_Check "ifnot" condition
    ifElement.clear();
    if ((*cit)->getAttributes().get(ASW("ifnot",5), ifElement))
    {
      if (ifElement.getSize() > 0)
      {
        //a_Check condition, if not met continue with next template
        if (context.useModel().exists(ifElement))
        {
          if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
            context.useEventVisitor().startEvent(ARope("Skipping conditional file template IFNOT ")+ifElement, AEventVisitor::EL_DEBUG);
          continue;
        }
      }
    }

    //
    // Now we check if this is inlined or filename
    //
    AString str(1024, 512);
    if ((*cit)->getName().equals(AOS_BaseModules_Constants::TEMPLATE))
    {
      // Add inline template
      str.clear();
      (*cit)->emitContent(str);
      AFile_AString strfile(str);
      pTemplate->clear();
      pTemplate->fromAFile(strfile);
      pTemplate->process(context.useLuaTemplateContext(), context.useOutputBuffer());
    }
    else if ((*cit)->getName().equals(AOS_BaseModules_Constants::FILENAME))
    {
      // Add filename based template
      AFilename filename(m_Services.useConfiguration().getAosBaseDataDirectory());
      str.clear();
      (*cit)->emitContent(str);
      filename.join(str, false);

      AAutoPtr<ATemplate> pT(NULL, false);
      if (ACacheInterface::NOT_FOUND == m_Services.useCacheManager().getTemplate(context, filename, pT))
      {
        //a_Not found add error and continue
        if (context.useEventVisitor().isLogging(AEventVisitor::EL_WARN))
          context.useEventVisitor().startEvent(ARope(getClass())+ASWNL(": Unable to find a template file: ")+filename+ASWNL(", ignoring and continuing"), AEventVisitor::EL_WARN);
        continue;
      }

      //a_Parse template
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
        context.useEventVisitor().startEvent(ARope("Processing template file: ")+filename, AEventVisitor::EL_DEBUG);
      AASSERT(this, pT.isNotNull());
      pT->process(context.useLuaTemplateContext(), context.useOutputBuffer());
    }
  }

  return AOSContext::RETURN_OK;
}
