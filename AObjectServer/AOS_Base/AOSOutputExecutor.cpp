#include "pchAOS_Base.hpp"
#include "AOSOutputExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSOutputGeneratorInterface.hpp"
#include "AOSServices.hpp"
#include "AFile_Physical.hpp"
#include "AOSConfiguration.hpp"

#ifdef __DEBUG_DUMP__
void AOSOutputExecutor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSOutputExecutor @ " << std::hex << this << std::dec << ") {" << std::endl;

  OutputGeneratorContainer::const_iterator cit = m_OutputGenerators.begin();
  ADebugDumpable::indent(os, indent+1) << "m_OutputGenerators={" << std::endl;
  while (cit != m_OutputGenerators.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit).second->getClass() << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

const AString& AOSOutputExecutor::getClass() const
{
  static const AString CLASS("AOSOutputExecutor");
  return CLASS;
}

void AOSOutputExecutor::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  OutputGeneratorContainer::const_iterator cit = m_OutputGenerators.begin();
  while (cit != m_OutputGenerators.end())
  {
    addProperty(eBase, ASW("generator",9), (*cit).second->getClass());
    ++cit;
  }
}

AOSOutputExecutor::AOSOutputExecutor(AOSServices& services) :
  m_Services(services)
{
  registerAdminObject(m_Services.useAdminRegistry());
}

AOSOutputExecutor::~AOSOutputExecutor()
{
  try
  {
    OutputGeneratorContainer::iterator it = m_OutputGenerators.begin();
    while (it != m_OutputGenerators.end())
    {
      (*it).second->deinit();
      pDelete((*it).second);
      ++it;
    }
  } catch(...) {}
}

void AOSOutputExecutor::registerOutputGenerator(AOSOutputGeneratorInterface *pGenerator)
{
  if (!pGenerator)
    ATHROW(this, AException::InvalidParameter);

  const AString& command = pGenerator->getClass();
#ifdef __DEBUG_DUMP__
  std::cerr << "Output command registration: " << command << std::endl;
#endif

  OutputGeneratorContainer::iterator it = m_OutputGenerators.find(command);
  if (it != m_OutputGenerators.end())
  {
    //a_Command already has this processor
    m_Services.useLog().add(ASWNL("AOSOutputExecutor::registerOutputGenerator:replacing output generator"), (*it).first, command, ALog::WARNING);
    delete (*it).second;
    (*it).second = pGenerator;
  }
  else
  {
    //a_Add new command
    m_Services.useLog().add(ASWNL("AOSOutputExecutor::registerOutputGenerator"), command, ALog::INFO);
    m_OutputGenerators[command] = pGenerator;
  }

  //a_Initialize it
  pGenerator->init();

  //a_Register the module's admin interface
  pGenerator->registerAdminObject(m_Services.useAdminRegistry(), ASW("AOSOutputExecutor",17));
}

void AOSOutputExecutor::execute(AOSContext& context)
{
  static const AString OVERRIDE_OUTPUT("overrideOutput",14);

  AString command;
  if (
    !context.useRequestHeader().useUrl().getParameterPairs().get(OVERRIDE_OUTPUT, command) &&
     m_Services.useConfiguration().isOutputOverrideAllowed()
  )
  {
    command = context.getOutputCommand();
  }
  else if (command.equals("NOP"))
  {
    //a_If NOP was used force XML
    context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
  }

  if (command.isEmpty())
  {
    if (!m_Services.useConfiguration().getAosDefaultOutputGenerator().isEmpty())
    {
      command.assign(m_Services.useConfiguration().getAosDefaultOutputGenerator());
    }
    else
    {
      context.setExecutionState(ASW("No output generator, defaulting to XML",38));
      return;
    }
  }

  try
  {
    //a_Find input command, if not found execute the default
    OutputGeneratorContainer::iterator it = m_OutputGenerators.find(command);
    if (it == m_OutputGenerators.end())
    {
      context.setExecutionState(ARope("Skipping unknown output generator: ",35)+command);
      m_Services.useLog().add(AString("AOSOutputExecutor::execute:Skipping unknown output generator"), command, ALog::WARNING);
    }
    else
    {
      if (context.useContextFlags().isSet(AOSContext::CTXFLAG_IS_AJAX))
      {
        //a_Generate AJAX output
        context.setExecutionState(ARope("Generating AJAX output: ",24)+(*it).first);

        //a_Start timer
        ATimer timer(true);
        if (!(*it).second->execute(context))
        {
          context.addError((*it).second->getClass()+"::execute", "Returned false");
          return;
        }

        //a_Add execution time
        (*it).second->addExecutionTimeSample(timer.getInterval());

        //a_Event over
        context.useEventVisitor().reset();
      }
      else
      {
        //a_Generate output
        context.setExecutionState(ARope("Generating output: ",19)+(*it).first);

        //a_Start timer
        ATimer timer(true);

        AXmlElement& e = context.useOutputRootXmlElement().addElement(ASW("execute/output", 14), command);

        //a_Publish total execution time before output is generated
        context.useOutputRootXmlElement().addElement(ASW("request_time",12), context.getRequestTimer());
        context.useOutputRootXmlElement().addElement(ASW("context_time",12), context.getContextTimer());

        //a_Execute output
        if (!(*it).second->execute(context))
        {
          context.addError((*it).second->getClass()+"::execute", "Returned false");
          return;
        }

        //a_Add execution time
        double interval = timer.getInterval();
        e.addAttribute(ASW("timer",5), AString::fromDouble(interval));
        (*it).second->addExecutionTimeSample(interval);

        //a_Event over
        context.useEventVisitor().reset();
      }
    }
  }
  catch(AException& ex)
  {
    AString strWhere("AOSOutputExecutor::execute(", 27);
    strWhere.append(command);
    strWhere.append(')');
    context.addError(strWhere, ex.what());

    AXmlElement& element = context.useOutputRootXmlElement().addElement("output_error");
    element.addElement("where", strWhere);
    element.addElement("exception", ex);
  }
  catch(...)
  {
    AString strWhere("AOSOutputExecutor::execute(", 27);
    strWhere.append(command);
    strWhere.append(')');
    context.addError(strWhere, "Unknown Exception");

    context.useOutputRootXmlElement().addElement("output_error").addData("Unknown Exception");
  }
}
