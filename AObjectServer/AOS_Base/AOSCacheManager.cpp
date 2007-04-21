#include "pchAOS_Base.hpp"
#include "AOSCacheManager.hpp"
#include "ACriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"

#ifdef __DEBUG_DUMP__
void AOSCacheManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSCacheManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_StaticFileCache={" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  m_StaticFileCache.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

const AString& AOSCacheManager::getClass() const
{
  static const AString CLASS("AOSCacheManager");
  return CLASS;
}

void AOSCacheManager::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::addAdminXml(eBase, request);

}

AOSCacheManager::AOSCacheManager(AOSServices& services) :
  m_Services(services)
{
  registerAdminObject(m_Services.useAdminRegistry());
}

AOSCacheManager::~AOSCacheManager()
{
}

AFile *AOSCacheManager::getStaticFile(const AFilename& filename)
{
  return m_StaticFileCache.get(filename);
}
