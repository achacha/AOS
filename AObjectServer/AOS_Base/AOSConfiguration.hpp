#ifndef INCLUDED__AOSConfiguration_HPP__
#define INCLUDED__AOSConfiguration_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSDirectoryConfig.hpp"
#include "ABitArray.hpp"
#include "AFilename.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"

class AUrl;
class AFile_Socket;
class AOSCommand;
class ASynchronization;
class ALog;
class AOSServices;

/*!
Global configuration
*/
class AOS_BASE_API AOSConfiguration : public AOSAdminInterface
{
public:
  AOSConfiguration(const AFilename& baseDir, AOSServices&);
  virtual ~AOSConfiguration();

  /*!
  Initialize static structures of this class (used in reporting actual hostname and port)
  App server may be behind a load balancer or web server and may need to report something else
  This is not required and localhost, 80, 443 is assumed by default
  server is the HTTP header server identifier
  */
  static void initializeStaticHostInfo(const AString& hostname, int http_port, int https_port, const AString& serverId);

  /*!
  Database
  */
  static const AString DATABASE_URL;
  static const AString DATABASE_CONNECTIONS;

  /*!
  Configuration XML document's root element
  Read only
  */
  const AXmlElement& getConfigRoot() const;
  
  /*!
  Configuration XML document's root element
  Modifiable
  */
  AXmlElement& useConfigRoot();

  /*!
  Base directory
  */
  const AFilename& getBaseDir() const;

  /*!
  Load configuration for a given module
  */
  void loadConfig(const AString&);

  //a_Control flags
  enum ConfigBit
  {
    DisableXslCache = 0   //!<  Disable for XSL cache, when off XSL files are read from disk every time
  };

  /*!
  Setting of configuration bits
  */
  void setConfigBits(ConfigBit bit, bool state = true);

  /*!
  Access to configuration bitmap
  */
  const ABitArray& getConfigBits() const;

  /*!
  Convert a given URl to the reported values
  server and port are used as per configuration
  */
  void convertUrlToReportedServerAndPort(AUrl& url) const;

  /*!
  Reported hostname and ports
  Used when load balanced and a common DNS entry is desired
  */
  const AString& getReportedHostname() const;
  int getReportedHttpPort() const;
  int getReportedHttpsPort() const;
  void setReportedHostname(const AString& configPath);
  void setReportedHttpPort(const AString& configPath);
  void setReportedHttpsPort(const AString& configPath);

  /*!
  Reported Server: value in the HTTP response header
  */
  const AString& getReportedServer() const;
  void setReportedServer(const AString& configPath);

  /*!
  Admin base directory
  */
  const AFilename& getAdminBaseHttpDir() const;

  /*!
  Default filename
  */
  void setAosDefaultFilename(const AString&);
  const AString& getAosDefaultFilename() const;

  /*!
  Input/Output defaults
  */
  const AString& getAosDefaultInputProcessor() const;
  void setAosDefaultInputProcessor(const AString&);
  const AString& getAosDefaultOutputGenerator() const;
  void setAosDefaultOutputGenerator(const AString&);

  /*!
  Map mimetype from extension
  */
  bool getMimeTypeFromExt(AString ext, AString& mimeType) const;  //a_true if ext to MIME type mapping exists
  
  /*!
  Paths
  */
  const AFilename& getAosBaseConfigDirectory() const;    // Location of the config files
  const AFilename& getAosBaseDynamicDirectory() const;   // Commands
  const AFilename& getAosBaseDataDirectory() const;      // Data dir that is base for all command filename parameters
  const AFilename& getAosBaseStaticDirectory() const;    // Filesystem (fallback if command not found)

  /*!
  Check is dumpContext is allowed
  */
  bool isDumpContextAllowed() const;
  
  /*!
  Check if outputOverride is allowed
  */
  bool isOutputOverrideAllowed() const;

  /*!
  Dynamic module libraries
  */
  u4 getDynamicModuleLibraries(LIST_AString&) const;
  
  /*!
  Commands
  */
  const AOSCommand* const getCommand(const AUrl&) const;

  /*!
  Directory config
  */
  const AOSDirectoryConfig* const getDirectoryConfig(const AUrl&) const;

  /*!
  Context manager parameters
  @deprecated
  */
  size_t getAOSContextManagerHistoryMaxSize() const;
  size_t getAOSContextManagerFreestoreMaxSize() const;

  /*!
  Admin interface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Services
  AOSServices& m_Services;

  //a_Directory where all info lives for this instance
  AFilename m_BaseDir;

  //a_Configuration bits and Ini profile
  ABitArray m_ConfigBits;

  //a_Commands
  typedef std::map<AString, AOSCommand *> MAP_ASTRING_COMMANDPTR;
  MAP_ASTRING_COMMANDPTR m_CommandPtrs;
  
  //a_Directory configs
  //a_Maps absolute path (relative to dynamic root) to AXmlDocument with config
  typedef std::map<AString, AOSDirectoryConfig *> MAP_ASTRING_CONFIG;
  MAP_ASTRING_CONFIG m_DirectoryConfigs;

  /*!
  Load commands
  */
  void _loadCommands();
  void _readCommand(AFilename&);               //a_May alter the passed filename
  void _readDirectoryConfig(AFilename&);       //a_May alter the passed filename
  void _postProcessCommandAndConfig(LIST_AFilename&);

  //a_MIME type lookup
  MAP_AString_AString m_ExtToMimeType;
  void _readMIMETypes();

  //a_AOS directories from base
  AFilename m_AosBaseConfigDir;
  AFilename m_AosBaseStaticDir;
  AFilename m_AosBaseDynamicDir;
  AFilename m_AosBaseDataDir;
  
  //a_Internals
  AString m_AosDefaultFilename;
  AString m_AosDefaultInputProcessor;
  AString m_AosDefaultOutputGenerator;
  
  //a_Admin
  AFilename m_AdminBaseHttpDir;

  //a_XML document with all config files overlayed based on load order
  AXmlDocument m_Config;

  //a_Reported statics
  AString m_ReportedServer;
  AString m_ReportedHostname;
  int m_ReportedHttpPort;
  int m_ReportedHttpsPort;
};

#endif //INCLUDED__AOSConfiguration_HPP__
