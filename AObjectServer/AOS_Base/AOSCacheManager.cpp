#include "pchAOS_Base.hpp"
#include "AOSCacheManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"
#include "AOSContext.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"

const AString AOSCacheManager::STATIC_CACHE_ENABLED("/config/server/cache/enabled");

#ifdef __DEBUG_DUMP__
void AOSCacheManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSCacheManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_StaticFileCache={" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  mp_StaticFileCache->debugDump(os, indent+2);
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

  AASSERT(this, mp_StaticFileCache);

  {
    AXmlElement& elem = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("static_file_cache",17));

    addPropertyWithAction(
      elem, 
      ASW("enabled",7), 
      AString::fromBool(m_Services.useConfiguration().getBool(STATIC_CACHE_ENABLED, false)),
      ASW("Update",6), 
      ASWNL("Enable static content caching(1) or disable and clear(0)"),
      ASW("Set",3)
    );

    addProperty(elem, ASW("byte_size",9), AString::fromSize_t(mp_StaticFileCache->getByteSize()));  
    addProperty(elem, ASW("item_count",10), AString::fromSize_t(mp_StaticFileCache->getItemCount()));  
    
    size_t hit = mp_StaticFileCache->getHit();
    size_t miss = mp_StaticFileCache->getMiss();
    addProperty(elem, ASW("hit",3), AString::fromSize_t(hit));
    addProperty(elem, ASW("miss",4), AString::fromSize_t(miss));
    if (hit > 0 || miss > 0)
    {
      addProperty(elem, ASW("efficiency",10), AString::fromDouble((double)hit/double(hit+miss)));
    }
  }
}

void AOSCacheManager::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("property",8), str))
  {
    //a_Toggle cache state and clear when disabled
    if (str.equals(ASW("AOSCacheManager.static_file_cache.enabled",41)))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        if (str.equals("0"))
        {
          //a_Clear and disable
          m_Services.useConfiguration().setBool(STATIC_CACHE_ENABLED, false);
          mp_StaticFileCache->clear();
        }
        else
        {
          //a_Enable
          m_Services.useConfiguration().setBool(STATIC_CACHE_ENABLED, true);
        }
      }
    }
  }
}

AOSCacheManager::AOSCacheManager(AOSServices& services) :
  m_Services(services)
{
  int maxItems = m_Services.useConfiguration().getInt("/config/server/cache/max_items", 20000);
  int maxFileSizeInK = m_Services.useConfiguration().getInt("/config/server/cache/max_filesize", 512 * 1024);
  int cacheCount = m_Services.useConfiguration().getInt("/config/server/cache/cache_count", 97);
  mp_StaticFileCache = new ACache_FileSystem(maxItems, maxFileSizeInK * 1024, cacheCount);

  registerAdminObject(m_Services.useAdminRegistry());
}

AOSCacheManager::~AOSCacheManager()
{
}

bool AOSCacheManager::getStaticFile(AOSContext& context, const AFilename& filename, AAutoPtr<AFile>& pFile)
{
  AASSERT(this, mp_StaticFileCache);
  if (m_Services.useConfiguration().getBool(AOSCacheManager::STATIC_CACHE_ENABLED, false))
  {
    //a_Get from cache
    return mp_StaticFileCache->get(filename, pFile);
  }
  else
  {
    //a_No caching, read fom file system
    if (AFileSystem::isA(filename, AFileSystem::File))
    {
      context.setExecutionState(ARope("Reading physical file: ",25)+filename.toAString());
      pFile.reset(new AFile_Physical(filename, "rb"));
      pFile->open();
      return true;
    }
    else
    {
      pFile.reset(NULL);
      return false;
    }
  }
}
