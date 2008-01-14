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

void AOSContextQueueInterface::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);
}

void AOSContextQueueInterface::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
}
