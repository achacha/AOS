#include "pchAOS_Base.hpp"
#include "AOSModuleExecutor.hpp"
#include "AOSContext.hpp"
#include "ATimer.hpp"
#include "AOSModuleInterface.hpp"
#include "AOSServices.hpp"
#include "AOSCommand.hpp"

void AOSModuleExecutor::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModuleExecutor @ " << std::hex << this << std::dec << ") {" << std::endl;

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

  AString command = pModule->getClass();
  {
    AString str("  Module Registered: ");
    str.append(command);
    AOS_DEBUGTRACE(str.c_str(), NULL);
  }

  ModuleContainer::iterator it = m_Modules.find(command);
  if (it != m_Modules.end())
  {
    //a_Command already has modules
    m_Services.useLog().add(AString("AOSModuleExecutor::registerModule:replacing module"), (*it).first, command, ALog::WARNING);
    (*it).second->deinit();
    delete (*it).second;
    (*it).second = pModule;
  }
  else
  {
    //a_Add new command
    m_Services.useLog().add(AString("AOSModuleExecutor::registerModule"), command, ALog::INFO);
    m_Modules[command] = pModule;
  }

  //a_Initialize it
  pModule->init();

  //a_Register the module's admin interface
  pModule->adminRegisterObject(m_Services.useAdminRegistry(), ASW("AOSModuleExecutor",17));
}

void AOSModuleExecutor::execute(AOSContext& context, const AOSModules& modules)
{
  AOSModules::LIST_AOSMODULE_PTRS::const_iterator cit = modules.get().begin();
  int i = 0;
  while(cit != modules.get().end())
  {
    try
    {
      ModuleContainer::iterator it = m_Modules.find((*cit)->getModuleClass());
      if (it == m_Modules.end())
      {
        context.setExecutionState(ARope("Skipping unknown module: ",25)+(*cit)->getModuleClass());
        m_Services.useLog().add(AString("AOSModuleExecutor::execute:Skipping unknown module"), (*cit)->getModuleClass(), ALog::WARNING);
      }
      else
      {
        //a_Start timer and execute module
        context.setExecutionState(ARope("Executing module: ",18)+(*cit)->getModuleClass());

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
            context.addError((*cit)->getModuleClass()+"::execute", "Returned false");
          return;

          case AOSContext::RETURN_REDIRECT:
            context.useContextFlags().setBit(AOSContext::CTXFLAG_IS_REDIRECTING);
            context.useEventVisitor().set(ARope("Redirect detected for module: ",30)+(*it).second->getClass());
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
      context.addError(strWhere, "UnknownException");    //a_This will og it also
    }
    ++cit;
  }
}
