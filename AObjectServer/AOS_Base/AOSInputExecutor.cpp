#include "pchAOS_Base.hpp"
#include "AOSInputExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSInputProcessorInterface.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"

#ifdef __DEBUG_DUMP__
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
#endif

void AOSInputExecutor::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

  InputProcessorContainer::const_iterator cit = m_InputProcessors.begin();
  while (cit != m_InputProcessors.end())
  {
    addProperty(eBase, ASW("processor",9), (*cit).second->getClass());
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
  registerAdminObject(m_Services.useAdminRegistry());
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
#ifdef __DEBUG_DUMP__
  std::cerr << "Input command registration: " << command << std::endl;
#endif

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
  pProcessor->registerAdminObject(m_Services.useAdminRegistry(), ASW("AOSInputExecutor",16));
}

void AOSInputExecutor::execute(AOSContext& context)
{
  static const AString OVERRIDE_INPUT("overrideInput",13);
  
  AString command;
  if (!context.useRequestHeader().useUrl().useParameterPairs().get(OVERRIDE_INPUT, command))
  {
    command = context.getInputCommand();
  }

  if (command.isEmpty())
  {
    if (m_Services.useConfiguration().getAosDefaultInputProcessor().isEmpty())
      return;                                 //a_Do nothing, no command and no default
    else
      command.assign(m_Services.useConfiguration().getAosDefaultInputProcessor());
  }

  try
  {
    //a_Find input command, if not found execute the default
    InputProcessorContainer::iterator it = m_InputProcessors.find(command);
    if (it == m_InputProcessors.end())
    {
      m_Services.useLog().append(AString("AOSInputExecutor::execute: Skipping unknown module '")+command+"'");
    }
    else
    {
      if (context.useContextFlags().isSet(AOSContext::CTXFLAG_IS_AJAX))
      {
        //a_Process AJAX input
        context.setExecutionState(ARope("Processing AJAX input: ",23)+command);

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
        //a_Process input
        context.setExecutionState(ARope("Processing input: ",18)+command);

        AXmlElement& e = context.useModel().addElement(ASW("execute/input",13), command);
        ATimer timer(true);

        //a_Execute INPUT
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
