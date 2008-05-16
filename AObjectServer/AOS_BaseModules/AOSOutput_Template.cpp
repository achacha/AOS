#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_Template.hpp"
#include "ATemplate.hpp"
#include "AFile_Physical.hpp"
#include "AException.hpp"
#include "ASync_Mutex.hpp"
#include "ALock.hpp"
#include "ATemplateNodeHandler_LUA.hpp"

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

const AString& AOSOutput_Template::getClass() const
{
  static const AString CLASS("Template");
  return CLASS;
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
  AXmlElement::CONST_CONTAINER templateNames;
  context.getOutputParams().find(ASW("filename", 8), templateNames);
  if(templateNames.size() == 0)
  {
    context.addError(getClass(), ASWNL("AOSOutput_Template: Unable to find 'output/filename' parameter"));
    return AOSContext::RETURN_ERROR;
  }

  //a_Iterate filename types and execute/output templates
  bool templatesDisplayed = 0;
  for (AXmlElement::CONST_CONTAINER::const_iterator cit = templateNames.begin(); cit != templateNames.end(); ++cit)
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
          context.useEventVisitor().startEvent(ARope("Skipping conditional template for ")+ifElement, AEventVisitor::EL_DEBUG);
          continue;
        }
      }
    }
    ifElement.clear();
    if ((*cit)->getAttributes().get(ASW("ifnot",5), ifElement))
    {
      if (ifElement.getSize() > 0)
      {
        //a_Check condition, if not met continue with next template
        if (context.useModel().exists(ifElement))
        {
          context.useEventVisitor().startEvent(ARope("Skipping conditional template for ")+ifElement, AEventVisitor::EL_DEBUG);
          continue;
        }
      }
    }
   
    //a_Process and output template
    AFilename filename(m_Services.useConfiguration().getAosBaseDataDirectory());
    AString str(1024, 512);
    (*cit)->emitContent(str);
    filename.join(str, false);

    AAutoPtr<ATemplate> pTemplate;
    if (ACacheInterface::NOT_FOUND == m_Services.useCacheManager().getTemplate(context, filename, pTemplate))
    {
      //a_Not found add error and continue
      context.useEventVisitor().startEvent(ARope(getClass())+ASWNL(": Unable to find a template file: ")+filename+ASWNL(", ignoring and continuing"), AEventVisitor::EL_WARN);
      continue;
    }

    //a_Process template
    AASSERT(this, pTemplate.isNotNull());
    ABasePtrContainer objects;
    ATemplateContext tctx(objects, context.useModelXmlDocument(), context.useOutputBuffer());
    objects.insert(AOSContext::OBJECTNAME, &context);
    pTemplate->process(tctx);
    ++templatesDisplayed;
  }

  if (!templatesDisplayed)
  {
    context.addError(getClass(), ARope("None of the template filenames matched the conditions, nothing was generated for output"));
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}
