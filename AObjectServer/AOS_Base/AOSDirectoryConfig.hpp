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
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

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
  virtual void adminRegisterObject(AOSAdminRegistry& registry);
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;
  static const AString CLASS;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Path of the config
  AString m_Path;

  //a_Associated modules and the parameters
  AOSModules m_Modules;

  //a_The log
  ALog& m_Log;
};

#endif // INCLUDED__AOSDirectoryConfig_HPP__
