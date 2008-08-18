/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSOutputExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSOutputGeneratorInterface.hpp"
#include "AOSServices.hpp"

void AOSOutputExecutor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

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

  {
    AString str("  Output Generator Registered: ");
    str.append(command);
    AOS_DEBUGTRACE(str.c_str(), NULL);
  }

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
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
    {
      ARope rope("Default output generator overridden to: ",40);
      rope.append(command);
      context.useEventVisitor().startEvent(rope, AEventVisitor::EL_DEBUG);
    }
  }

  if (command.equals("NOP"))
  {
    //a_If NOP was used force XML
    m_Services.useConfiguration().setMimeTypeFromExt(ASW("xml",3), context);
    if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
      context.useEventVisitor().startEvent(ASWNL("NOP detected, defaulting to XML output"), AEventVisitor::EL_DEBUG);
  }

  if (command.isEmpty())
  {
    if (!m_Services.useConfiguration().getAosDefaultOutputGenerator().isEmpty())
    {
      command.assign(m_Services.useConfiguration().getAosDefaultOutputGenerator());
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
      {
        ARope rope("No output generator specified, defaulting to: ",46);
        rope.append(command);
        context.useEventVisitor().startEvent(rope, AEventVisitor::EL_DEBUG);
      }
    }
    else
    {
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
        context.useEventVisitor().startEvent(ASW("No output generator, defaulting to XML",38), AEventVisitor::EL_DEBUG);
      return;
    }
  }

  try
  {
    //a_Find input command, if not found execute the default
    OutputGeneratorContainer::iterator it = m_OutputGenerators.find(command);
    if (it == m_OutputGenerators.end())
    {
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_WARN))
      {
        ARope rope("Skipping unknown output generator: ",35);
        rope.append(command);
        context.useEventVisitor().startEvent(rope, AEventVisitor::EL_WARN);
      }
    }
    else
    {
      ATimer timer(true);

      //a_Generate output
      if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
      {
        ARope rope("Generating output: ",19);
        rope.append((*it).first);
        context.useEventVisitor().startEvent(rope, AEventVisitor::EL_INFO);
      }

      if (context.useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
      {
        context.useModel().overwriteElement(ASW("execute/output", 14)).addData(command);

        //a_Publish timers
        context.getRequestTimer().emitXml(context.useModel().overwriteElement(ASW("request_time",12)));
        context.getContextTimer().emitXml(context.useModel().overwriteElement(ASW("context_time",12)));
      }

      //a_Generate output
      AOSContext::ReturnCode ret = (*it).second->execute(context);
      switch (ret)
      {
        case AOSContext::RETURN_OK:
        break;

        case AOSContext::RETURN_REDIRECT:
          if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
          {
            context.useEventVisitor().startEvent(ASWNL("Output generator has done a redirect"), AEventVisitor::EL_DEBUG);
          }
        break;
      
        default:
          context.addError((*it).second->getClass(), ASWNL("Output generator returned neither OK nor REDIRECT"));
        return;
      }

      //a_Event over
      context.useEventVisitor().endEvent();

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
    context.addError(strWhere, ASWNL("Unknown Exception"));

    context.useModel().addElement("output_error").addData("Unknown Exception");
  }
}
