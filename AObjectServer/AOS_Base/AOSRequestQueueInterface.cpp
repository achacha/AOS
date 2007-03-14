#include "pchAOS_Base.hpp"
#include "AOSRequest.hpp"
#include "AOSRequestQueueInterface.hpp"
#include "AOSServices.hpp"

AOSRequestQueueInterface::AOSRequestQueueInterface(
  AOSServices& services,
  AOSRequestQueueInterface *pForward, // = NULL, 
  AOSRequestQueueInterface *pBack     // = NULL
) :
  m_AOSServices(services),
  mp_Forward(pForward),
  mp_Back(pBack)
{

}

AOSRequestQueueInterface::~AOSRequestQueueInterface()
{
}

void AOSRequestQueueInterface::_goForward(AOSRequest *pRequest)
{
  if (mp_Forward)
  {
    mp_Forward->add(pRequest);

    static const AString LOG_MSG("AOSRequestQueueInterface::_goForward");
    m_AOSServices.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);
  }
  else
    AOSRequest::deallocate(pRequest);
}

void AOSRequestQueueInterface::_goBack(AOSRequest *pRequest)
{
  if (mp_Back)
  {
    mp_Back->add(pRequest);

    static const AString LOG_MSG("AOSRequestQueueInterface::_goBack");
    m_AOSServices.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);
  }
  else
    AOSRequest::deallocate(pRequest);
}

void AOSRequestQueueInterface::setForwardQueueSet(AOSRequestQueueInterface *p)
{
  mp_Forward = p;
}

void AOSRequestQueueInterface::setBackQueueSet(AOSRequestQueueInterface *p)
{
  mp_Back = p;
}
