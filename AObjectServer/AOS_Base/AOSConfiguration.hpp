/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSConfiguration_HPP__
#define INCLUDED__AOSConfiguration_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSDirectoryConfig.hpp"
#include "ABitArray.hpp"
#include "AFileSystem.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"

class AUrl;
class AFile_Socket;
class AOSController;
class ASynchronization;
class ALog;
class AOSServices;
class AOSContext;

/*!
Global configuration
*/
class AOS_BASE_API AOSConfiguration : public AOSAdminInterface
{
public:
  //! Number of times to retry reading first char in HTTP header
  static int FIRST_CHAR_RETRIES;
  //! Initial sleep time waiting on reading first char
  static int SLEEP_STARTTIME;
  //! Increment the sleep every retry if first char is not read
  static int SLEEP_INCREMENT;
  //! If HTTP pipelining is enabled
  static bool IS_HTTP_PIPELINING_ENABLED;
  //! If gzip compression is enabled
  static bool GZIP_IS_ENABLED;
  //! Minimum size of content to compress
  static size_t GZIP_MIN_SIZE;
  //! Default gzip compression level
  static int GZIP_DEFAULT_LEVEL;
  //! Sleep time when flushing data and socket is unavailable
  static int UNAVAILABLE_SLEEP_TIME;
  //! Number of retries before considering socket unavailable for too long
  static int UNAVAILABLE_RETRIES;

public:
  /*!
  ctor

  @param baseDir where aos_root directory is to be found
  @param services reference to AOSServices object
  */
  AOSConfiguration(const AFilename& baseDir, AOSServices& services);
  
  //! virtual dtor
  virtual ~AOSConfiguration();

  /*!
  Initialize static structures of this class (used in reporting actual hostname and port)
  App server may be behind a load balancer or web server and may need to report something else
  This is not required and localhost, 80, 443 is assumed by default

  @param hostname to attach to
  @param http_port port for http listener
  @param https_port port for secure http listener
  @param serverId Name of the server that is used in HTTP response header in Server: tag
  */
  static void initializeStaticHostInfo(const AString& hostname, int http_port, int https_port, const AString& serverId);

  //! Path to the database URL
  static const AString DATABASE_URL;

  //! Path to the database connection to create per URL
  static const AString DATABASE_CONNECTIONS;

  /*!
  Configuration XML document's root element
  Read only

  @return constant reference to AXmlElement that is the configuration root
  */
  const AXmlElement& getConfigRoot() const;
  
  /*!
  Configuration XML document's root element
  Modifiable

  @return reference to AXmlElement that is the configuration root
  */
  AXmlElement& useConfigRoot();

  /*!
  Base directory

  @return reference to AFilename
  */
  const AFilename& getBaseDir() const;

  /*!
  Load configuration for a given module in config directory

  Given "MyModule" loads ./aos_root/config/MyModule.xml

  @param name of the module config to load
  */
  void loadConfig(const AString& name);

  /*!
  Convert a given URl to the reported values
  server and port are used as per configuration

  @param url to convert
  */
  void convertUrlToReportedServerAndPort(AUrl& url) const;

  /*!
  Reported hostname
  Used when load balanced and a common DNS entry is desired
  
  @return constant reference to reported hostname
  */
  const AString& getReportedHostname() const;
  
  /*!
  Reported HTTP port
  Used when load balanced and a common DNS entry is desired
  
  @return reported HTTP port
  */
  int getReportedHttpPort() const;

  /*!
  Reported HTTPS port
  Used when load balanced and a common DNS entry is desired
  
  @return reported HTTPS port
  */
  int getReportedHttpsPort() const;

  /*!
  Set reported hostname

  @param configPath path in the config file to use content as hostname
  */
  void setReportedHostname(const AString& configPath);

  /*!
  Set reported port

  @param configPath path in the config file to use content as port
  */
  void setReportedHttpPort(const AString& configPath);

  /*!
  Set reported secure port

  @param configPath path in the config file to use content as port
  */
  void setReportedHttpsPort(const AString& configPath);

  /*!
  Reported 'Server:' value in the HTTP response header
  
  @return constant reference to the server name
  */
  const AString& getReportedServer() const;

  /*!
  Set server name

  @param configPath path in the config file to use content as server name
  */
  void setReportedServer(const AString& configPath);

  /*!
  Admin base directory

  @return constant reference to the AFilename
  */
  const AFilename& getAdminBaseHttpDir() const;

  /*!
  Set default filename

  @param filename to use as default (index.html is default)
  */
  void setAosDefaultFilename(const AString& filename);

  /*!
  Get default filename

  @return constant reference to the AFilename
  */
  const AString& getAosDefaultFilename() const;

  /*!
  Get default input processor registered class name

  @return constant reference to the AFilename
  */
  const AString& getAosDefaultInputProcessor() const;
  
  /*!
  Set default input processor

  @param name of the registered input processor (no default)
  */
  void setAosDefaultInputProcessor(const AString& name);

  /*!
  Get default ouput generator registered class name

  @return constant reference to the AFilename
  */
  const AString& getAosDefaultOutputGenerator() const;
  
  /*!
  Set default output generator

  @param name of the registered output generator (xml is default)
  */
  void setAosDefaultOutputGenerator(const AString& name);

  /*!
  Get the default mime type when it cannot be matched with extension

  @return configured default MIME type
  */
  const AString& getDefaultMimeType() const;
  
  /*!
  Map mime type from extension
  
  @param ext Extension to try and map
  @param target appended with extension if found
  @return true if extension is mapped and value was found, false if extension is not mapped
  */
  bool getMimeTypeFromExt(const AString& ext, AString& target) const;
  
  /*!
  Set the mime type based on extension or use configured default if extension is not found
  
  @param ext Extension to try and map, if not found will use default
  @param context AOSContext contains the HTTP response header that will get set
  */
  void setMimeTypeFromExt(const AString& ext, AOSContext& context);

  /*!
  Set the MIME type in response header based on the extension found in the request header's URL
  NOTE: If extension mapping not found then default is set

  @param context AOSContext contains both request and response header
  */
  void setMimeTypeFromRequestExt(AOSContext& context);
  
  /*!
  Location of the config directory

  @return constant reference to AFilename object
  */
  const AFilename& getAosBaseConfigDirectory() const;

  /*!
  Location of the dynamic directory (controllers)

  @return constant reference to AFilename object
  */
  const AFilename& getAosBaseDynamicDirectory() const;

  /*!
  Location of the data directory
  Data dir that is base for all command filename parameters

  @return constant reference to AFilename object
  */
  const AFilename& getAosBaseDataDirectory() const;

  /*!
  Location of the static directory
  Static filesystem (fallback if controller not found)

  @return constant reference to AFilename object
  */
  const AFilename& getAosBaseStaticDirectory() const;

  /*!
  Check is dumpContext is allowed in the configuration file

  @return true if allowed
  */
  bool isDumpContextAllowed() const;
  
  /*!
  Check if outputOverride is allowed in the configuration file

  @return true if allowed
  */
  bool isOutputOverrideAllowed() const;

  /*!
  Dynamic module libraries to load as per configuration file

  @param libs list to append to
  */
  u4 getDynamicModuleLibraries(LIST_AString& libs) const;
  
  /*!
  Controller for a given URL

  Returned pointer should never be deleted by the caller

  @return constant pointer AOSController for a given URL
  */
  const AOSController* const getController(const AUrl&) const;

  /*!
  Directory config for a given URL

  Returned pointer should never be deleted by the caller

  @return constant pointer AOSDirectoryConfig for a given URL
  */
  const AOSDirectoryConfig* const getDirectoryConfig(const AUrl&) const;

  /*!
  Get a set of extensions that are to be compressed based on config
  
  @return constant reference to a set of extension that should be compressed
  */
  const SET_AString& getCompressedExtensions() const;

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

  //a_Controllers
  typedef std::map<AString, AOSController *> MAP_ASTRING_CONTROLLERPTR;
  MAP_ASTRING_CONTROLLERPTR m_ControllerPtrs;
  
  //a_Directory configs
  //a_Maps absolute path (relative to dynamic root) to AXmlDocument with config
  typedef std::map<AString, AOSDirectoryConfig *> MAP_ASTRING_CONFIG;
  MAP_ASTRING_CONFIG m_DirectoryConfigs;

  //a_Extensions that should be compressed by default (assuming min size criterion is met)
  SET_AString m_CompressedExtensions;

  //a_Initialize statics
  void _initStatics();

  //a_Populate the extension set
  void _populateGzipCompressionExtensions();

  /*!
  Load commands
  */
  void _loadControllers();
  void _readController(AFilename&);               //a_May alter the passed filename
  void _readDirectoryConfig(AFilename&);          //a_May alter the passed filename
  void _postProcessControllerAndConfig(AFileSystem::FileInfos &);

  //a_MIME type lookup
  MAP_AString_AString m_ExtToMimeType;
  void _readMIMETypes();

  //a_Default MIME type
  AString m_DefaultMimeType;

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
