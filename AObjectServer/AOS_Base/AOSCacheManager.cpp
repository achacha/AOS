/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSCacheManager.hpp"
#include "ALock.hpp"
#include "AOSServices.hpp"
#include "AOSContext.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ATextOdometer.hpp"

const AString AOSCacheManager::CLASS("AOSCacheManager");

void AOSCacheManager::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_StaticFileCache={" << std::endl;
  mp_StaticFileCache->debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_DataFileCache={" << std::endl;
  mp_DataFileCache->debugDump(os, indent+2);
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

const AString& AOSCacheManager::getClass() const
{
  return CLASS;
}

void AOSCacheManager::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSAdminInterface::adminEmitXml(eBase, request);

  AASSERT(this, mp_StaticFileCache);

  {
    AXmlElement& elem = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("static_file_cache",17));

    adminAddPropertyWithAction(
      elem, 
      ASW("enabled",7), 
      AString::fromBool(m_IsStaticFileCacheEnabled),
      ASW("Update",6), 
      ASWNL("1:Enable static content caching, 0:Disable and clear, -1:Clear only"),
      ASW("Set",3)
    );

    adminAddProperty(elem, ASW("byte_size",9), AString::fromSize_t(mp_StaticFileCache->getByteSize()));  
    adminAddProperty(elem, ASW("item_count",10), AString::fromSize_t(mp_StaticFileCache->getItemCount()));  
    
    size_t hit = mp_StaticFileCache->getHit();
    size_t miss = mp_StaticFileCache->getMiss();
    adminAddProperty(elem, ASW("hit",3), AString::fromSize_t(hit));
    adminAddProperty(elem, ASW("miss",4), AString::fromSize_t(miss));
    if (hit > 0 || miss > 0)
    {
      adminAddProperty(elem, ASW("efficiency",10), AString::fromDouble((double)hit/double(hit+miss)));
    }
  }

  {
    AXmlElement& elem = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("data_file_cache",15));

    adminAddPropertyWithAction(
      elem, 
      ASW("enabled",7), 
      AString::fromBool(m_IsDataFileCacheEnabled),
      ASW("Update",6), 
      ASWNL("1:Enable static content caching, 0:Disable and clear, -1:Clear only"),
      ASW("Set",3)
    );

    adminAddProperty(elem, ASW("byte_size",9), AString::fromSize_t(mp_DataFileCache->getByteSize()));  
    adminAddProperty(elem, ASW("item_count",10), AString::fromSize_t(mp_DataFileCache->getItemCount()));  
    
    size_t hit = mp_DataFileCache->getHit();
    size_t miss = mp_DataFileCache->getMiss();
    adminAddProperty(elem, ASW("hit",3), AString::fromSize_t(hit));
    adminAddProperty(elem, ASW("miss",4), AString::fromSize_t(miss));
    if (hit > 0 || miss > 0)
    {
      adminAddProperty(elem, ASW("efficiency",10), AString::fromDouble((double)hit/double(hit+miss)));
    }
  }

  {
    AXmlElement& elem = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("template_cache",14));

    adminAddPropertyWithAction(
      elem, 
      ASW("enabled",7), 
      AString::fromBool(m_IsTemplateCacheEnabled),
      ASW("Update",6), 
      ASWNL("1:Enable template caching, 0:Disable and clear, -1:Clear only"),
      ASW("Set",3)
    );

    adminAddProperty(elem, ASW("item_count",10), AString::fromSize_t(mp_TemplateCache->size()));  
  }
}

void AOSCacheManager::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
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
          m_IsStaticFileCacheEnabled = false;
          mp_StaticFileCache->clear();  //a_Cache uses internal sync
        }
        else if (str.equals("-1"))
        {
          mp_StaticFileCache->clear();  //a_Cache uses internal sync
        }
        else
        {
          //a_Enable
          m_IsStaticFileCacheEnabled = true;
        }
      }
    }
    else if (str.equals(ASW("AOSCacheManager.template_cache.enabled",38)))
    {
      str.clear();
      if (request.getUrl().getParameterPairs().get(ASW("Set",3), str))
      {
        if (str.equals("0"))
        {
          //a_Clear and disable
          m_IsTemplateCacheEnabled = false;
          
          ALock lock(m_TemplateSync);
          mp_TemplateCache->clear();
        }
        else if (str.equals("-1"))
        {
          ALock lock(m_TemplateSync);
          mp_TemplateCache->clear();
        }
        else
        {
          //a_Enable
          m_IsTemplateCacheEnabled = true;
        }
      }
    }
  }
}

AOSCacheManager::AOSCacheManager(AOSServices& services) :
  m_Services(services)
{
  {
    m_IsStaticFileCacheEnabled = m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/server/static-file-cache/enabled",40), false);
    int maxItems = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/static-file-cache/max-items", 20000);
    int maxFileSizeInK = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/static-file-cache/max-filesize", 512 * 1024);
    int cacheCount = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/static-file-cache/cache-count", 13);
    mp_StaticFileCache = new ACache_FileSystem(maxItems, maxFileSizeInK * 1024, cacheCount);
  }

  {
    m_IsDataFileCacheEnabled = m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/server/data-file-cache/enabled",40), false);
    int maxItems = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/data-file-cache/max-items", 20000);
    int maxFileSizeInK = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/data-file-cache/max-filesize", 512 * 1024);
    int cacheCount = m_Services.useConfiguration().useConfigRoot().getInt("/config/server/data-file-cache/cache-count", 13);
    mp_DataFileCache = new ACache_FileSystem(maxItems, maxFileSizeInK * 1024, cacheCount);
  }

  //a_Status template cache
  mp_StatusTemplateCache = new STATUS_TEMPLATE_CACHE();

  //a_Parsed template cache
  mp_TemplateCache = new TEMPLATE_CACHE();
  m_IsTemplateCacheEnabled = m_Services.useConfiguration().useConfigRoot().getBool(ASW("/config/server/template-cache/enabled", 37), true);

  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSCacheManager::~AOSCacheManager()
{
  try
  {
    delete mp_StaticFileCache;
    delete mp_DataFileCache;
    
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

ACacheInterface::STATUS AOSCacheManager::getStaticFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle)
{
  ATime modified;
  return getStaticFile(context, fname, handle, modified);
}

ACacheInterface::STATUS AOSCacheManager::getStaticFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle, ATime& modified)
{
  AASSERT(this, mp_StaticFileCache);

  LIST_AFilename directories;
  m_Services.useConfiguration().getAosStaticDirectoryChain(context.useRequestHeader(), directories);
  for (LIST_AFilename::iterator it = directories.begin(); it != directories.end(); ++it)
  {
    it->join(fname);
    if (m_IsStaticFileCacheEnabled)
    {
      //a_Get from cache
      const ATime& ifModifiedSince = context.useRequestHeader().getIfModifiedSince();
      ACacheInterface::STATUS status = mp_StaticFileCache->get(*it, handle, modified, ifModifiedSince);
      if (ACacheInterface::NOT_FOUND == status)
        continue;
      else
        return status;
    }
    else
    {
      //a_No caching, read from file system
      if (AFileSystem::isA(*it, AFileSystem::File))
      {
        // Found existing file
        context.useEventVisitor().startEvent(ARope("Reading physical file: ",23)+*it);
        handle.reset(new AFile_Physical(*it));
        handle->open();
        if (!AFileSystem::getLastModifiedTime(*it, modified))
          ATHROW_EX(&context, AException::NotFound, *it);     // Why did isA think it was a good file?

        return ACacheInterface::FOUND_NOT_CACHED;
      }
    }
  }

  handle.reset(NULL);
  return ACacheInterface::NOT_FOUND;
}

ACacheInterface::STATUS AOSCacheManager::getDataFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle)
{
  ATime modified;
  return getDataFile(context, fname, handle, modified);
}

ACacheInterface::STATUS AOSCacheManager::getDataFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle, ATime& modified)
{
  AASSERT(this, mp_DataFileCache);

  LIST_AFilename directories;
  m_Services.useConfiguration().getAosDataDirectoryChain(context.useRequestHeader(), directories);
  for (LIST_AFilename::iterator it = directories.begin(); it != directories.end(); ++it)
  {
    it->join(fname);
    if (m_IsDataFileCacheEnabled)
    {
      //a_Get from cache
      const ATime& ifModifiedSince = context.useRequestHeader().getIfModifiedSince();
      ACacheInterface::STATUS status = mp_DataFileCache->get(*it, handle, modified, ifModifiedSince);
      if (ACacheInterface::NOT_FOUND == status)
        continue;
      else
        return status;
    }
    else
    {
      //a_No caching, read fom file system
      if (AFileSystem::isA(*it, AFileSystem::File))
      {
        context.useEventVisitor().startEvent(ARope("Reading physical file: ",23)+*it);
        handle.reset(new AFile_Physical(*it));
        handle->open();
        if (!AFileSystem::getLastModifiedTime(*it, modified))
          ATHROW_EX(&context, AException::NotFound, *it);

        return ACacheInterface::FOUND_NOT_CACHED;
      }
    }
  }

  handle.reset(NULL);
  return ACacheInterface::NOT_FOUND;
}

ACacheInterface::STATUS AOSCacheManager::getTemplate(AOSContext& context, const AFilename& filename, AAutoPtr<ATemplate>& pTemplate)
{
  AASSERT(this, mp_TemplateCache);
  if (m_IsTemplateCacheEnabled)
  {
    
    ALock lock(m_TemplateSync);
    TEMPLATE_CACHE::iterator it = mp_TemplateCache->find(filename);
    if (mp_TemplateCache->end() == it)
    {
      //a_Not found in cache, try to read and parse
      if (AFileSystem::exists(filename))
      {
        //a_File exists, parse and cache
        AAutoPtr<ATemplate> pNewTemplate(m_Services.createTemplate(), true);
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
      pTemplate.reset(m_Services.createTemplate(), true);
      AFile_Physical file(filename);
      file.open();
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
  AAutoPtr<ATemplate> p(m_Services.createTemplate(), true);
  p->fromAFile(file);

  (*mp_StatusTemplateCache)[key] = p.use();
  
  p.setOwnership(false);
  return p.use();
}
