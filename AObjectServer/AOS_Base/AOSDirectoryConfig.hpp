#ifndef INCLUDED__AOSDirectoryConfig_HPP__
#define INCLUDED__AOSDirectoryConfig_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "AXmlParsable.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSCommand.hpp"
#include "AOSModules.hpp"

class AFile;
class ALog;

class AOS_BASE_API AOSDirectoryConfig : public AXmlEmittable, public AOSAdminInterface, public AXmlParsable
{
public:
  AOSDirectoryConfig(const AString path, const AXmlElement& base, ALog& alog);
  virtual ~AOSDirectoryConfig();

  /*!
  Parse from XML element
  */
  virtual void fromXml(const AXmlElement&);

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

  /*!
  Get module container
  */
  const AOSModules& getModules() const;

  /*!
  Access to the path
  */
  const AString& getPath() const;
  
  /*!
  AOSAdminInterface
  */
  virtual void registerAdminObject(AOSAdminRegistry& registry);
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;
  static const AString CLASS;

private:
  //! Path of the config
  AString m_Path;

  //a_Associated modules and the parameters
  AOSModules m_Modules;

  //a_The log
  ALog& m_Log;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSDirectoryConfig_HPP__
