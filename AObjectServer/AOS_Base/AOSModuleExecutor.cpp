#include "pchAOS_Base.hpp"
#include "AOSModuleExecutor.hpp"
#include "AOSContext.hpp"
#include "ATimer.hpp"
#include "AOSModuleInterface.hpp"
#include "AOSServices.hpp"
#include "AOSController.hpp"

void AOSModuleExecutor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ModuleContainer::const_iterator cit = m_Modules.begin();
  ADebugDumpable::indent(os, indent+1) << "m_Modules={" << std::endl;
  while (cit != m_Modules.end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit).second->getClass() << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

const AString& AOSModuleExecutor::getClass() const
{
  static const AString CLASS("AOSModuleExecutor");
  return CLASS;
}

void AOSModuleExecutor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  ModuleContainer::const_iterator cit = m_Modules.begin();
  while (cit != m_Modules.end())
  {
    adminAddProperty(eBase, ASW("module",6), (*cit).second->getClass());
    ++cit;
  }
}

AOSModuleExecutor::AOSModuleExecutor(AOSServices& services) :
  m_Services(services)
{
  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSModuleExecutor::~AOSModuleExecutor()
{
  try
  {
    ModuleContainer::iterator it = m_Modules.begin();
    while (it != m_Modules.end())
    {
      (*it).second->deinit();
      pDelete((*it).second);
      ++it;
    }
  } catch(...) {}
}

void AOSModuleExecutor::registerModule(AOSModuleInterface *pModule)
{
  if (!pModule)
    ATHROW(this, AException::InvalidParameter);

  AString controller = pModule->getClass();
  {
    AString str("  Controller registered: ");
    str.append(controller);
    AOS_DEBUGTRACE(str.c_str(), NULL);
  }

  ModuleContainer::iterator it = m_Modules.find(controller);
  if (it != m_Modules.end())
  {
    //a_Controller already has modules
    m_Services.useLog().add(AString("AOSModuleExecutor::registerModule:replacing controller"), (*it).first, controller, ALog::WARNING);
    (*it).second->deinit();
    delete (*it).second;
    (*it).second = pModule;
  }
  else
  {
    //a_Add new command
    m_Services.useLog().add(AString("AOSModuleExecutor::registerModule"), controller, ALog::INFO);
    m_Modules[controller] = pModule;
  }

  //a_Initialize it
  pModule->init();

  //a_Register the module's admin interface
  pModule->adminRegisterObject(m_Services.useAdminRegistry(), ASW("AOSModuleExecutor",17));
}

void AOSModuleExecutor::execute(AOSContext& context, const AOSModules& modules)
{
  int i = 0;
  for (AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = modules.get().begin(); cit != modules.get().end(); ++cit)
  {
    try
    {
      ModuleContainer::iterator it = m_Modules.find((*cit)->getModuleClass());
      if (it == m_Modules.end())
      {
        if (context.useEventVisitor().isLogging(AEventVisitor::EL_WARN))
        {
          ARope rope("Skipping unknown module: ",25);
          rope.append((*cit)->getModuleClass());
          context.useEventVisitor().startEvent(rope, AEventVisitor::EL_WARN);
        }
      }
      else
      {
        //a_Check if the module should execute based on this context
        if (!(*cit)->isExecute(context))
        {
          if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
          {
            ARope rope("Skipping (condition) module: ",29);
            rope.append((*cit)->getModuleClass());
            context.useEventVisitor().startEvent(rope, AEventVisitor::EL_DEBUG);
          }
          continue;
        }

        //a_Start timer and execute module
        if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
        {
          ARope rope("Executing module: ",18);
          rope.append((*cit)->getModuleClass());
          context.useEventVisitor().startEvent(rope, AEventVisitor::EL_INFO);
        }

        if (context.useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
        {
          AXmlElement& e = context.useModel().overwriteElement(ASW("execute",7)).addElement(ASW("module",6));
          e.addAttribute(ASW("seq",3), AString::fromInt(i++));
          e.addData((*cit)->getModuleClass());
        }

        ATimer timer(true);
        AOSContext::ReturnCode retCode = (*it).second->execute(context, (*cit)->useParams());
        switch (retCode)
        {
          case AOSContext::RETURN_ERROR:
            //a_Error occured
            context.addError((*cit)->getModuleClass()+ASW("::execute",9), ASWNL("Returned false"));
          return;

          case AOSContext::RETURN_REDIRECT:
            context.useContextFlags().setBit(AOSContext::CTXFLAG_IS_REDIRECTING);
            if (context.useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            {
              ARope rope("Redirect detected for module: ",30);
              rope.append((*it).second->getClass());
              context.useEventVisitor().startEvent(rope, AEventVisitor::EL_INFO);
            }
          return;
        }
      }
    }
    catch(AException& ex)
    {
      AString strWhere("AOSModuleExecutor::execute(", 27);
      if (*cit)
        strWhere.append((*cit)->getModuleClass());
      else
        strWhere.append("NULL",4);
      strWhere.append(')');
      context.addError(strWhere, ex.what());   //a_This will log it also
    }
    catch(...)
    {
      AString strWhere("AOSModuleExecutor::execute(", 27);
      if (*cit)
        strWhere.append((*cit)->getModuleClass());
      else
        strWhere.append("NULL",4);
      strWhere.append(')');
      context.addError(strWhere, ASWNL("UnknownException"));    //a_This will og it also
    }
  }
}
