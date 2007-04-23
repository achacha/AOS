#ifndef INCLUDED__AOSCacheManager_HPP__
#define INCLUDED__AOSCacheManager_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ACache_FileSystem.hpp"
#include "ASync_CriticalSection.hpp"

class AOSServices;

class AOS_BASE_API AOSCacheManager : public AOSAdminInterface
{
public:
  AOSCacheManager(AOSServices&);
  virtual ~AOSCacheManager();
  
  /*!
  Use static content cache
  */
  AFile *getStaticFile(const AFilename&);

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& base, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //a_Reference to the services
  AOSServices& m_Services;

  //a_Static file cache
  ACache_FileSystem m_StaticFileCache;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSCacheManager_HPP__
