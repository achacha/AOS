#include "pchAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSServices.hpp"

AOSContextQueueInterface::AOSContextQueueInterface(
  AOSServices& services,
  AOSContextQueueInterface *pYes,  // = NULL, 
  AOSContextQueueInterface *pNo,   // = NULL
  AOSContextQueueInterface *pError // = NULL
) :
  m_Services(services),
  mp_Yes(pYes),
  mp_No(pNo),
  mp_Error(pNo)
{
}

AOSContextQueueInterface::~AOSContextQueueInterface()
{
}

void AOSContextQueueInterface::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);
}

void AOSContextQueueInterface::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
}

void AOSContextQueueInterface::_goYes(AOSContext *pContext)
{
  if (mp_Yes)
  {
    pContext->setExecutionState(AString(getClass())+ASW("::_goYes",8));

    mp_Yes->add(pContext);
  }
  else
  {
    try
    {
      m_Services.useContextManager().deallocate(pContext);
    }
    catch(AException& e)
    {
      m_Services.useLog().add(AString(getClass())+ASW("::_goYes",8), *pContext, ALog::FAILURE);
      m_Services.useLog().addException(e);
    }
    catch(...)
    {
      m_Services.useLog().add(AString(getClass())+ASW("::_goYes",8)+ASW(":Unknown exception",18), *pContext, ALog::FAILURE);
    }
  }
}

void AOSContextQueueInterface::_goNo(AOSContext *pContext)
{
  if (mp_No)
  {
    pContext->setExecutionState(AString(getClass())+ASW("::_goNo",7));

    mp_No->add(pContext);
  }
  else
  {
    try
    {
      m_Services.useContextManager().deallocate(pContext);
    }
    catch(AException& e)
    {
      m_Services.useLog().add(AString(getClass())+ASW("::_goNo",7), *pContext, ALog::FAILURE);
      m_Services.useLog().addException(e);
    }
    catch(...)
    {
      m_Services.useLog().add(AString(getClass())+ASW("::_goNo",7)+ASW(":Unknown exception",18), *pContext, ALog::FAILURE);
    }
  }
}

void AOSContextQueueInterface::_goError(AOSContext *pContext)
{
  if (mp_Error)
  {
    pContext->setExecutionState(AString(getClass())+ASW("::_goError",10));

    mp_Error->add(pContext);
  }
  else
  {
    try
    {
      m_Services.useContextManager().deallocate(pContext);
    }
    catch(AException& e)
    {
      m_Services.useLog().add(AString(getClass())+ASW("::_goError",10), *pContext, ALog::FAILURE);
      m_Services.useLog().addException(e);
    }
    catch(...)
    {
      m_Services.useLog().add(AString(getClass())+ASW("::_goError",10)+ASW(":Unknown exception",18), *pContext, ALog::FAILURE);
    }
  }
}

void AOSContextQueueInterface::_goTerminate(AOSContext *pContext)
{
  pContext->setExecutionState(AString(getClass())+ASW("::_goTerminate",14));
  try
  {
    m_Services.useContextManager().deallocate(pContext);
  }
  catch(AException& e)
  {
    m_Services.useLog().add(AString(getClass())+ASW("::_goTerminate",14), *pContext, ALog::FAILURE);
    m_Services.useLog().addException(e);
  }
  catch(...)
  {
    m_Services.useLog().add(AString(getClass())+ASW("::_goTerminate",14)+ASW(":Unknown exception",18), *pContext, ALog::FAILURE);
  }
}

void AOSContextQueueInterface::setYesContextQueue(AOSContextQueueInterface *p)
{
  mp_Yes = p;
}

void AOSContextQueueInterface::setNoContextQueue(AOSContextQueueInterface *p)
{
  mp_No = p;
}

void AOSContextQueueInterface::setErrorContextQueue(AOSContextQueueInterface *p)
{
  mp_Error = p;
}
