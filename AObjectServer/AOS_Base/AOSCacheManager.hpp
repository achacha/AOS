#ifndef INCLUDED__AOSCacheManager_HPP__
#define INCLUDED__AOSCacheManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ACache_FileSystem.hpp"
#include "ASync_CriticalSection.hpp"

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
  Use static content cache
  If cache enabled will get item fro cache and auto ptr will be set on no-delete
  If not caching or file too large then auto ptr will auto delete the item when out of scope

  Returns true is a file is found (cached or otherwise)
  */
  bool getStaticFile(AOSContext&, const AFilename&, AAutoPtr<AFile>&);

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

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSCacheManager_HPP__
