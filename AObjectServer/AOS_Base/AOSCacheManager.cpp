#include "pchAOS_Base.hpp"
#include "AOSCacheManager.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSConfiguration.hpp"
#include "AOSContext.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ATextOdometer.hpp"
#include "ATemplateNodeHandler_CODE.hpp"
#include "ATemplateNodeHandler_LUA.hpp"

const AString AOSCacheManager::STATIC_CACHE_ENABLED("/config/server/static-file-cache/enabled");
const AString AOSCacheManager::TEMPLATE_CACHE_ENABLED("/config/server/template-cache/enabled");

#ifdef __DEBUG_DUMP__
void AOSCacheManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSCacheManager @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_StaticFileCache={" << std::endl;
  mp_StaticFileCache->debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  if (mp_StatusTemplateCache)
  {
    ADebugDumpable::indent(os, indent+1) << "mp_StatusTemplateCache={" << std::endl;
    for(STATUS_TEMPLATE_CACHE::iterator it = mp_StatusTemplateCache->begin(); it != mp_StatusTemplateCache->end(); ++it)
    {
      ADebugDumpable::indent(os, indent+2) << it->first << "=" << AString::fromPointer(it->second) << std::endl;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }

  if (mp_TemplateCache)
  {
    ADebugDumpable::indent(os, indent+1) << "mp_TemplateCache={" << std::endl;
    for(TEMPLATE_CACHE::iterator it = mp_TemplateCache->begin(); it != mp_TemplateCache->end(); ++it)
    {
      ADebugDumpable::indent(os, indent+2) << it->first << "=" << AString::fromPointer(it->second) << std::endl;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }

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
      AString::fromBool(m_Services.useConfiguration().useConfigRoot().getBool(STATIC_CACHE_ENABLED, false)),
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
          m_Services.useConfiguration().useConfigRoot().setBool(STATIC_CACHE_ENABLED, false);
          mp_StaticFileCache->clear();
        }
        else
        {
          //a_Enable
          m_Services.useConfiguration().useConfigRoot().setBool(STATIC_CACHE_ENABLED, true);
        }
      }
    }
  }
}

AOSCacheManager::AOSCacheManager(AOSServices& services) :
  m_Services(services)
{
  int maxItems = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/cache/max_items", 20000);
  int maxFileSizeInK = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/cache/max_filesize", 512 * 1024);
  int cacheCount = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/cache/cache_count", 97);
  mp_StaticFileCache = new ACache_FileSystem(maxItems, maxFileSizeInK * 1024, cacheCount);

  //a_Status template cache
  mp_StatusTemplateCache = new STATUS_TEMPLATE_CACHE();

  //a_Parsed template cache
  mp_TemplateCache = new TEMPLATE_CACHE();

  registerAdminObject(m_Services.useAdminRegistry());
}

AOSCacheManager::~AOSCacheManager()
{
  try
  {
    delete mp_StaticFileCache;
    
    {
      //a_Release status templates
      for(STATUS_TEMPLATE_CACHE::iterator it = mp_StatusTemplateCache->begin(); it != mp_StatusTemplateCache->end(); ++it)
        delete (*it).second;
      delete mp_StatusTemplateCache;
    }

    {
      //a_Release parsed templates
      for(TEMPLATE_CACHE::iterator it = mp_TemplateCache->begin(); it != mp_TemplateCache->end(); ++it)
        delete (*it).second;
      delete mp_TemplateCache;
    }
  }
  catch(...) {}
}

ACacheInterface::STATUS AOSCacheManager::getStaticFile(AOSContext& context, const AFilename& filename, AAutoPtr<AFile>& pFile, ATime& modified, const ATime& ifModifiedSince)
{
  AASSERT(this, mp_StaticFileCache);
  if (m_Services.useConfiguration().useConfigRoot().getBool(AOSCacheManager::STATIC_CACHE_ENABLED, false))
  {
    //a_Get from cache
    return mp_StaticFileCache->get(filename, pFile, modified, ifModifiedSince);
  }
  else
  {
    //a_No caching, read fom file system
    if (AFileSystem::isA(filename, AFileSystem::File))
    {
      context.setExecutionState(ARope("Reading physical file: ",25)+filename.toAString());
      pFile.reset(new AFile_Physical(filename, "rb"));
      pFile->open();
      if (!AFileSystem::getLastModifiedTime(filename, modified))
        ATHROW_EX(&context, AException::NotFound, filename);

      return ACacheInterface::FOUND_NOT_CACHED;
    }
    else
    {
      pFile.reset(NULL);
      return ACacheInterface::NOT_FOUND;
    }
  }
}

ACacheInterface::STATUS AOSCacheManager::getTemplate(AOSContext& context, const AFilename& filename, AAutoPtr<ATemplate>& pTemplate)
{
  AASSERT(this, mp_TemplateCache);
  if (m_Services.useConfiguration().useConfigRoot().getBool(AOSCacheManager::TEMPLATE_CACHE_ENABLED, false))
  {
    TEMPLATE_CACHE::iterator it;
    
    {
      ALock lock(m_TemplateSync);
      it = mp_TemplateCache->find(filename);
    }

    if (mp_TemplateCache->end() == it)
    {
      //a_Not found in cache, try to read and parse
      ALock lock(m_TemplateSync);
      if (AFileSystem::exists(filename))
      {
        //a_File exists, parse and cache
        AAutoPtr<ATemplate> pNewTemplate(m_Services.createTemplate());
        AFile_Physical file(filename);
        file.open();
        pNewTemplate->fromAFile(file);
        (*mp_TemplateCache)[filename] = pNewTemplate.use();
        pNewTemplate.setOwnership(false);
        pTemplate.reset(pNewTemplate.use(), false);
        return ACacheInterface::FOUND;
      }
      else
      {
        //a_File does not exist, insert a NULL into cache
        (*mp_TemplateCache)[filename] = NULL;
        pTemplate.reset();
        return ACacheInterface::NOT_FOUND;
      }
    }
    else
    {
      //a_Exists in the cache
      pTemplate.reset(it->second, false);
      if (pTemplate.isNull())
      {
        //a_File was not found originally
        return ACacheInterface::NOT_FOUND;
      }
      else
      {
        //a_Exists
        return ACacheInterface::FOUND;
      }
    }
  }
  else
  {
    if (AFileSystem::exists(filename))
    {
      //a_File exists, parse and return
      pTemplate.reset(m_Services.createTemplate());
      AFile_Physical file(filename);
      pTemplate->fromAFile(file);
      return ACacheInterface::FOUND_NOT_CACHED;
    }
    else
    {
      //a_File does not exist
      pTemplate.reset();
      return ACacheInterface::NOT_FOUND;
    }
  }
}

bool AOSCacheManager::getStatusTemplate(int statusCode, AAutoPtr<ATemplate>& pTemplate)
{
  AASSERT(this, mp_StatusTemplateCache);
  STATUS_TEMPLATE_CACHE::iterator it;
  
  {
    ALock lock(m_StatusTemplateSync);
    it = mp_StatusTemplateCache->find(statusCode);
  }

  if (mp_StatusTemplateCache->end() == it)
  {
    //a_Not found insert
    ALock lock(m_StatusTemplateSync);
    AString pathFilename("/config/server/error-templates/HTTP-",36);
    ATextOdometer odo(AString::fromInt(statusCode), 3);
    odo.emit(pathFilename);

    AFilename filename(
      m_Services.useConfiguration().getAosBaseDataDirectory(),
      m_Services.useConfiguration().useConfigRoot().getString(pathFilename, AConstant::ASTRING_EMPTY),
      false
    );

    //a_Load the template file and cache it
    if (!filename.useFilename().isEmpty() && AFileSystem::exists(filename))
    {
      pTemplate.reset(_putFileIntoStatusTemplateCache(statusCode, filename), false);
      return true;
    }
    else
    {
      //a_Template for this error is not found (may not exist)
      //a_Try to use default filee if available or use NULL if neither exists
      filename.clear();
      filename.set(m_Services.useConfiguration().getAosBaseDataDirectory(), true);
      filename.join(
        m_Services.useConfiguration().useConfigRoot().getString(
          ASW("/config/server/error-templates/default",38), 
          AConstant::ASTRING_EMPTY
        ),
        false
      );

      if (!filename.useFilename().isEmpty() && AFileSystem::exists(filename))
      {
        //a_Try to use default template
        pTemplate.reset(_putFileIntoStatusTemplateCache(statusCode, filename), false);
        return true;
      }
      else
      {
        //a_Really doesn't exist
        (*mp_StatusTemplateCache)[statusCode] = NULL;
        pTemplate.reset();
        return false;
      }
    }
  }
  else
  {
    //a_Put the pointer into the holder and remove ownership
    //a_NULL in iterator means the file does not exist and we already checked before
    pTemplate.reset((*it).second);
    pTemplate.setOwnership(false);
    
    return (NULL != (*it).second);
  }
}

ATemplate* AOSCacheManager::_putFileIntoStatusTemplateCache(int key, const AString& filename)
{
  AFile_Physical file(filename);
  file.open();

  //a_Process error template
  AAutoPtr<ATemplate> p(m_Services.createTemplate());
  p->fromAFile(file);

  (*mp_StatusTemplateCache)[key] = p.use();
  
  p.setOwnership(false);
  return p.use();
}
