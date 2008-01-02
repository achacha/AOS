#include "pchAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSServices.hpp"

AOSContextQueueInterface::AOSContextQueueInterface(
  AOSServices& services
) :
  m_Services(services)
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
