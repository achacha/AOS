#ifndef INCLUDED__AOSConfiguration_HPP__
#define INCLUDED__AOSConfiguration_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ABitArray.hpp"
#include "AThread.hpp"
#include "AINIProfile.hpp"
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
  AOSConfiguration(const AFilename& baseConfigDir, AOSServices&, ASynchronization&);
  virtual ~AOSConfiguration();

  /*!
  Returns true if exists
  */
  bool exists(const AString& path) const;

  /*!
  String corresponding to the path
  Returns true if exists
  */
  bool get(const AString& path, AOutputBuffer& target);

  /*!
  Configuration XML document's root element
  */
  const AXmlElement& getConfigRoot() const;
  
  /*!
  Base config directory
  */
  const AFilename& getBaseConfigDir() const;
  
  /*!
  Load commands
  */
  void loadCommands(AOSServices&);

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
  Global ini
  @deprecated
  */
  AINIProfile& useIni() { return m_Ini; }
  
  /*!
  Server specific parameters
  @deprecated
  */
  int getReportedServerPort() const;
  int getReportedSecureServerPort() const;
  AString getReportedServerName() const;
  bool getMimeTypeFromExt(AString, AString&) const;  //a_true if ext to MIME type mapping exists

  /*!
  Database(s)
  @deprecated
  */
  bool getDatabaseUrl(AUrl&) const;        //a_Returns false if none specified
  int  getDatabaseMaxConnections() const;  //a_Number of pooled connections to the database

  /*!
  HTTP header reading
  @deprecated
  */
  int getHttpFirstCharReadTries() const;

  /*!
  HTTP 1.1 pipelining enabled
  @deprecated
  */
  int isHttpPipeliningEnabled() const;

  /*!
  AOS server information
  @deprecated
  */
  int getAosServerPort() const;
  int getAosSecureServerPort() const;
  AString getAosCertFilename() const;
  AString getAosPKeyFilename() const;
  
  /*!
  Paths
  @deprecated
  */
  const AString& getAosBaseConfigDirectory() const;    // Location of the config files
  const AString& getAosBaseDynamicDirectory() const;   // Commands
  const AString& getAosBaseDataDirectory() const;      // Data dir that is base for all command filename parameters
  const AString& getAosBaseStaticDirectory() const;    // Filesystem (fallback if command not found)
  const AString& getAosDefaultFilename() const;        // Default filename

  /*!
  Admin server information
  @deprecated
  */
  int getAdminServerPort() const;
  const AString& getAdminBaseHttpDirectory() const;

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
  Input/Output defaults
  @deprecated
  */
  const AString& getAosDefaultInputProcessor() const;
  const AString& getAosDefaultOutputGenerator() const;

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

  //static constants
  //@deprecated
  static const AString CONFIG;

private:
  ASynchronization& m_ScreenSynch;
  
  //a_Services
  AOSServices& m_Services;

  //a_Directory where all config info lives for this instance
  AFilename m_BaseConfigDir;

  //a_Configuration bits and Ini profile
  ABitArray m_ConfigBits;
  
  //@deprecated
  AINIProfile m_Ini;

  //a_MIME type lookup
  MAP_AString_AString m_ExtToMimeType;
  void _readMIMETypes();

  //a_AOS
  //@deprecated
  AString m_AosBaseConfigDir;
  AString m_AosBaseStaticDir;
  AString m_AosBaseDynamicDir;
  AString m_AosBaseDataDir;
  AString m_AosDefaultFilename;
  AString m_AosDefaultInputProcessor;
  AString m_AosDefaultOutputGenerator;
  
  //a_Admin
  //@deprecated 
  AString m_AdminBaseHttpDir;

  //a_XML document with all config files overlayed based on load order
  AXmlDocument m_Config;

  //a_Commands
  typedef std::map<AString, AOSCommand *> MAP_ASTRING_COMMANDPTR;
  MAP_ASTRING_COMMANDPTR m_CommandPtrs;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSConfiguration_HPP__
