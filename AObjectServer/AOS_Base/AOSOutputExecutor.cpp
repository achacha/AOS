#include "pchAOS_Base.hpp"
#include "AOSOutputExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSOutputGeneratorInterface.hpp"
#include "AOSServices.hpp"
#include "AFile_Physical.hpp"
#include "AOSConfiguration.hpp"

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

const AString& AOSOutputExecutor::getClass() const
{
  static const AString CLASS("AOSOutputExecutor");
  return CLASS;
}

void AOSOutputExecutor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  OutputGeneratorContainer::const_iterator cit = m_OutputGenerators.begin();
  while (cit != m_OutputGenerators.end())
  {
    adminAddProperty(eBase, ASW("generator",9), (*cit).second->getClass());
    ++cit;
  }
}

AOSOutputExecutor::AOSOutputExecutor(AOSServices& services) :
  m_Services(services)
{
  adminRegisterObject(m_Services.useAdminRegistry());
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
  pGenerator->adminRegisterObject(m_Services.useAdminRegistry(), ASW("AOSOutputExecutor",17));
}

void AOSOutputExecutor::execute(AOSContext& context)
{
  static const AString OVERRIDE_OUTPUT("overrideOutput",14);

  AString command;
  if (
    context.useRequestParameterPairs().exists(OVERRIDE_OUTPUT)
    && m_Services.useConfiguration().isOutputOverrideAllowed()
  )
  {
    //a_Override requested and allowed
    context.useRequestParameterPairs().get(OVERRIDE_OUTPUT, command);
  }
  else
  {
    command = context.getOutputCommand();
    context.setExecutionState(ARope("Default output generator overridden to: ")+command);
  }

  if (command.equals("NOP"))
  {
    //a_If NOP was used force XML
    context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
    context.setExecutionState(ASWNL("NOP detected, defaulting to XML output"));
  }

  if (command.isEmpty())
  {
    if (!m_Services.useConfiguration().getAosDefaultOutputGenerator().isEmpty())
    {
      command.assign(m_Services.useConfiguration().getAosDefaultOutputGenerator());
      context.setExecutionState(ARope("No output generator specified, defaulting to: ")+command);
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
      ATimer timer(true);

      //a_Generate output
      context.setExecutionState(ARope("Generating output: ",19)+(*it).first);

      if (context.useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
      {
        AXmlElement& e = context.useModel().overwriteElement(ASW("execute/output", 14)).addData(command);

        //a_Publish timers
        context.getRequestTimer().emitXml(context.useModel().overwriteElement(ASW("request_time",12)));
        context.getContextTimer().emitXml(context.useModel().overwriteElement(ASW("context_time",12)));
      }

      //a_Generate output
      if (AOSContext::RETURN_OK != (*it).second->execute(context))
      {
        context.addError((*it).second->getClass(), "Output generator failed");
        return;
      }

      //a_Event over
      context.useEventVisitor().reset();

      //a_Add execution time
      (*it).second->addExecutionTimeSample(timer.getInterval());
    }
  }
  catch(AException& ex)
  {
    AString strWhere("AOSOutputExecutor::execute(", 27);
    strWhere.append(command);
    strWhere.append(')');
    context.addError(strWhere, ex.what());

    AXmlElement& element = context.useModel().addElement("output_error");
    element.addElement("where", strWhere);
    element.addElement("exception", ex);
  }
  catch(...)
  {
    AString strWhere("AOSOutputExecutor::execute(", 27);
    strWhere.append(command);
    strWhere.append(')');
    context.addError(strWhere, "Unknown Exception");

    context.useModel().addElement("output_error").addData("Unknown Exception");
  }
}
