#include "pchAOS_Base.hpp"
#include "AOSInputExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSInputProcessorInterface.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"

void AOSInputExecutor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSInputExecutor @ " << std::hex << this << std::dec << ") {" << std::endl;

  InputProcessorContainer::const_iterator cit = m_InputProcessors.begin();
  ADebugDumpable::indent(os, indent+1) << "m_InputProcessors={" << std::endl;
  while (cit != m_InputProcessors.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit).second->getClass() << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AOSInputExecutor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  InputProcessorContainer::const_iterator cit = m_InputProcessors.begin();
  while (cit != m_InputProcessors.end())
  {
    adminAddProperty(eBase, ASW("processor",9), (*cit).second->getClass());
    ++cit;
  }
}

const AString& AOSInputExecutor::getClass() const
{
  static const AString CLASS("AOSInputExecutor");
  return CLASS;
}

AOSInputExecutor::AOSInputExecutor(AOSServices& services) :
  m_Services(services)
{
  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSInputExecutor::~AOSInputExecutor()
{
  try
  {
    InputProcessorContainer::iterator it = m_InputProcessors.begin();
    while (it != m_InputProcessors.end())
    {
      (*it).second->deinit();
      pDelete((*it).second);
      ++it;
    }
  } catch(...) {}
}

void AOSInputExecutor::registerInputProcessor(AOSInputProcessorInterface *pProcessor)
{
  if (!pProcessor)
    ATHROW(this, AException::InvalidParameter);

  const AString& command = pProcessor->getClass();
  InputProcessorContainer::iterator it = m_InputProcessors.find(command);
  if (it != m_InputProcessors.end())
  {
    //a_Command already has this processor
    m_Services.useLog().add(AString("AOSInputExecutor::registerModule:replacing input processor"), (*it).first, command, ALog::WARNING);
    delete (*it).second;
    (*it).second = pProcessor;
  }
  else
  {
    //a_Add new command
    m_Services.useLog().add(AString("AOSInputExecutor::registerModule"), command, ALog::INFO);
    m_InputProcessors[command] = pProcessor;
  }

  //a_Initialize it
  pProcessor->init();

  //a_Register the module's admin interface
  pProcessor->adminRegisterObject(m_Services.useAdminRegistry(), ASW("AOSInputExecutor",16));
}

void AOSInputExecutor::execute(AOSContext& context)
{
  static const AString OVERRIDE_INPUT("overrideInput",13);
  
  AString command;
  if (!context.useRequestHeader().useUrl().useParameterPairs().get(OVERRIDE_INPUT, command))
  {
    command = context.getInputCommand();
    context.setExecutionState(ARope("Input processor: ")+command);
  }

  if (command.isEmpty())
  {
    if (m_Services.useConfiguration().getAosDefaultInputProcessor().isEmpty())
    {
      context.setExecutionState(ASW("No input processor", 18));
      return;                                 //a_Do nothing, no command and no default
    }
    else
    {
      command.assign(m_Services.useConfiguration().getAosDefaultInputProcessor());
      context.setExecutionState(ARope("No input processor, using default: ")+command);
    }
  }

  try
  {
    //a_Find input command, if not found execute the default
    InputProcessorContainer::iterator it = m_InputProcessors.find(command);
    if (it == m_InputProcessors.end())
    {
      m_Services.useLog().append(ARope("AOSInputExecutor::execute: Skipping unknown input processor '")+command+"'");
    }
    else
    {
      ATimer timer(true);
      AOSContext::ReturnCode retCode = (*it).second->execute(context);
      
      //a_Add execution time
      (*it).second->addExecutionTimeSample(timer.getInterval());

      switch (retCode)
      {
        case AOSContext::RETURN_ERROR:
          context.addError((*it).second->getClass()+"::execute", "Returned false");
          context.useEventVisitor().reset();
        break;

        case AOSContext::RETURN_REDIRECT:
          context.useContextFlags().setBit(AOSContext::CTXFLAG_IS_REDIRECTING);
          context.useEventVisitor().set(ASW("Redirect detected.",18));
          context.useEventVisitor().reset();
        break;

        default:
          context.useEventVisitor().reset();
        break;
      }
    }
  }
  catch(AException& ex)
  {
    AString strWhere("AOSInputExecutor::execute(", 26);
    strWhere.append(command);
    strWhere.append(')');
    context.addError(strWhere, ex.what());

    context.setExecutionState(strWhere, true);
  }
  catch(...)
  {
    AString strWhere("AOSInputExecutor::execute(", 26);
    strWhere.append(command);
    strWhere.append(')');
    context.addError(strWhere, "Unknown Exception");

    context.setExecutionState(strWhere, true);
  }
}
