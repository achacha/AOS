/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSCacheManager_HPP__
#define INCLUDED__AOSCacheManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ACache_FileSystem.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"
#include "ATemplate.hpp"

class AOSServices;
class AOSContext;

class AOS_BASE_API AOSCacheManager : public AOSAdminInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSCacheManager(AOSServices&);
  virtual ~AOSCacheManager();
  
  /*!
  Get static file from cache or try to load it into the cache
  NOTE: If cache enabled will get item from cache and auto ptr will be set on no-delete
  NOTE: If not caching or file too large then auto ptr will auto delete the item when out of scope

  @param context of the request for locale and modified-since info
  @param fname relative to the data directory
  @param handle [OUT] receives the handle to the file if found, this object will manage the deletion of the file if needed
  @param modified [OUT] receives the modified time for the file if found
  @return status, ACacheInterface::NOT_FOUND if not found, otherwise several cache specific status codes
  */
  ACacheInterface::STATUS getStaticFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle);
  ACacheInterface::STATUS getStaticFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle, ATime& modified);

  /*!
  Get data file from cache or try to load it into the cache
  NOTE: If cache enabled will get item from cache and auto ptr will be set on no-delete
  NOTE: If not caching or file too large then auto ptr will auto delete the item when out of scope
  
  @param context of the request for locale and modified-since info
  @param fname relative to the data directory
  @param handle [OUT] receives the handle to the file if found, this object will manage the deletion of the file if needed
  @param modified [OUT] receives the modified time for the file if found
  @return status, ACacheInterface::NOT_FOUND if not found, otherwise several cache specific status codes
  */
  ACacheInterface::STATUS getDataFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle);
  ACacheInterface::STATUS getDataFile(AOSContext& context, const AFilename& fname, ACache_FileSystem::HANDLE& handle, ATime& modified);

  /*!
  Get parsed ATemplate from cache or try to load it into the cache
  If cache enabled will get item from cache and auto ptr will be set on no-delete
  If not caching or file too large then auto ptr will auto delete the item when out of scope
  */
  ACacheInterface::STATUS getTemplate(AOSContext&, const AFilename&, AAutoPtr<ATemplate>&);

  /*!
  Get status code template from cache or try to load into the cache (separate cache than other templates)
  Auto pointer is set to no-delete and is a holder for the template obect in the cache

  Returns true only if a file entry is found and exists on the file system, false otherwise
  */
  bool getStatusTemplate(int statusCode, AAutoPtr<ATemplate>&);

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Reference to the services
  AOSServices& m_Services;

  //a_Static file cache
  ACache_FileSystem *mp_StaticFileCache;
  bool m_IsStaticFileCacheEnabled;

  //a_Data file cache
  ACache_FileSystem *mp_DataFileCache;
  bool m_IsDataFileCacheEnabled;

  //a_Parsed template cache
  typedef std::map<AFilename, ATemplate *> TEMPLATE_CACHE;
  TEMPLATE_CACHE *mp_TemplateCache;
  ASync_CriticalSectionSpinLock m_TemplateSync;
  bool m_IsTemplateCacheEnabled;

  //a_Status template cache
  typedef std::map<int, ATemplate *> STATUS_TEMPLATE_CACHE;
  STATUS_TEMPLATE_CACHE *mp_StatusTemplateCache;
  ASync_CriticalSectionSpinLock m_StatusTemplateSync;

  //a_Put file into status template cache
  ATemplate *_putFileIntoStatusTemplateCache(int key, const AString& filename);
};

#endif //INCLUDED__AOSCacheManager_HPP__
