#ifndef INCLUDED__AOSConfiguration_HPP__
#define INCLUDED__AOSConfiguration_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ABitArray.hpp"
#include "AThread.hpp"
#include "AFilename.hpp"
#include "AXmlDocument.hpp"

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
  AOSConfiguration(const AFilename& baseDir, AOSServices&, ASynchronization&);
  virtual ~AOSConfiguration();

  /*!
  Initialize static structures of this class (used in reporting actual hostname and port)
  App server may be behind a load balancer or web server and may need to report something else
  This is not required and localhost, 80, 443 is assumed by default
  server is the HTTP header server identifier
  */
  static void initializeStaticHostInfo(const AString& hostname, int http_port, int https_port, const AString& serverId);

  /*! Listener constants */
  static const AString LISTEN_ADMIN_PORT;    
  static const AString LISTEN_HTTP_PORT;     
  static const AString LISTEN_HTTPS_PORT;

  /*!
  Database
  */
  static const AString DATABASE_URL;
  static const AString DATABASE_CONNECTIONS;

  /*!
  Returns true if exists
  */
  bool exists(const AString& path) const;

  /*!
  Emitted data corresponding to the path
  Result is appended
  Returns true if exists
  */
  bool emitString(const AString& path, AOutputBuffer& target) const;

  /*!
  String corresponding to the path
  This is not the most efficient call due to temporary return object, but useful for initializing statics
  Returns emitted data into a string for a path or default
  */
  AString getString(const AString& path, const AString& strDefault) const;

  /*!
  String corresponding to the path
  Result is converted to int if does not exist default is returned
  */
  int getInt(const AString& path, int iDefault) const;

  /*!
  String corresponding to the path
  Case insensitive "true" or "1" are true
  Result is converted to bool if does not exist default is returned
  */
  bool getBool(const AString& path, bool boolDefault) const;

  /*!
  Configuration XML document's root element
  */
  const AXmlElement& getConfigRoot() const;
  
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
  void setConfigBits(ConfigBit bit, bool state = true) { m_ConfigBits.setBit(bit, state); }

  /*!
  Access to configuration bitmap
  */
  const ABitArray& getConfigBits() const { return m_ConfigBits; }

  /*!
  Server specific parameters
  Set methods will look up the config path and set value if found
  */
  const AString& getReportedServer() const;
  const AString& getReportedHostname() const;
  int getReportedHttpPort() const;
  int getReportedHttpsPort() const;
  void setReportedServer(const AString& configPath);
  void setReportedHostname(const AString& configPath);
  void setReportedHttpPort(const AString& configPath);
  void setReportedHttpsPort(const AString& configPath);

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
  Dynamic module libraries
  */
  u4 getDynamicModuleLibraries(LIST_AString&) const;
  
  /*!
  Commands
  */
  const AOSCommand* const getCommand(const AUrl&) const;
  void dumpCommands(AOutputBuffer&) const;                      //TODO: temporary until AOSAdmin is better

  /*!
  Context manager parameters
  @deprecated
  */
  size_t getAOSContextManagerHistoryMaxSize() const;
  size_t getAOSContextManagerFreestoreMaxSize() const;

  /*!
  Admin interface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  ASynchronization& m_ScreenSynch;
  
  //a_Services
  AOSServices& m_Services;

  //a_Directory where all info lives for this instance
  AFilename m_BaseDir;

  //a_Configuration bits and Ini profile
  ABitArray m_ConfigBits;

  /*!
  Load commands
  */
  void _loadCommands();

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

  //a_Commands
  typedef std::map<AString, AOSCommand *> MAP_ASTRING_COMMANDPTR;
  MAP_ASTRING_COMMANDPTR m_CommandPtrs;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSConfiguration_HPP__
