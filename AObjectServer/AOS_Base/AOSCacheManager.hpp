#ifndef INCLUDED__AOSCacheManager_HPP__
#define INCLUDED__AOSCacheManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ACache_FileSystem.hpp"
#include "ASync_CriticalSection.hpp"
#include "ATemplate.hpp"

class AOSServices;
class AOSContext;

class AOS_BASE_API AOSCacheManager : public AOSAdminInterface
{
public:
  static const AString STATIC_CACHE_ENABLED;  //a_Path to static cache enabled

public:
  AOSCacheManager(AOSServices&);
  virtual ~AOSCacheManager();
  
  /*!
  Get static file from cache or try to load it into the cache
  If cache enabled will get item from cache and auto ptr will be set on no-delete
  If not caching or file too large then auto ptr will auto delete the item when out of scope
  modified - returns the modified time for the file if found
  ifModifiedSince - is If-Modified: HTTP request header time
  */
  ACacheInterface::STATUS getStaticFile(AOSContext&, const AFilename&, AAutoPtr<AFile>&, ATime& modified, const ATime& ifModifiedSince);

  /*!
  Get status code template from cache or try to load into the cache
  Auto pointer is set to no-delete and is a holder for the template obect in the cache

  Returns true only if a file entry is found and exists on the file system, false otherwise
  */
  bool getStatusTemplate(int statusCode, AAutoPtr<ATemplate>&);

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //a_Reference to the services
  AOSServices& m_Services;

  //a_Static file cache
  ACache_FileSystem *mp_StaticFileCache;
  ATemplate *_putFileIntoStatusTemplateCache(int key, const AString& filename);

  //a_Template cache
  typedef std::map<int, ATemplate *> TEMPLATE_CACHE;
  TEMPLATE_CACHE *mp_TemplateCache;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSCacheManager_HPP__
