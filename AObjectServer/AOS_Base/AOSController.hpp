/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSController_HPP__
#define INCLUDED__AOSController_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "AXmlParsable.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSModuleInfo.hpp"
#include "AOSModules.hpp"

class AFile;
class ALog;

class AOS_BASE_API AOSController : public AXmlEmittable, public AXmlParsable, public AOSAdminInterface
{
public:
  //! Class name
  static const AString CLASS;

  /*!
  Constant strings
  */
  static const AString S_CONTROLLER;
  static const AString S_CONTROLLER_ROOT;
  static const AString S_INPUT;
  static const AString S_MODULE;
  static const AString S_OUTPUT;

  static const AString S_CLASS;
  static const AString S_ENABLED;
  static const AString S_AJAX;
  static const AString S_ALIAS;
  static const AString S_GZIP;
  static const AString S_SESSION;
  static const AString S_CACHECONTROLNOCACHE;

public:
  AOSController(const AString& name, ALog&);
  virtual ~AOSController();

  /*!
  Parse from XML element
  */
  virtual void fromXml(const AXmlElement&);

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Determines if a command is enabled
  Disabled commands will be skipped and static equivaltent will be checked
    if static not found then 404 results
  */
  bool isEnabled() const;
  
  /*!
  If this is an ajax command and to supress extra XML in the model
  Only what is added explicitly by commands is what you get
  */
  bool isForceAjax() const;
  
  /*!
  Checks if the dynamic command is forcing no cache
  This can be turned on by a controller config and browser will cache
  */
  bool isCacheControlNoCache() const;

  /*!
  Checks if the session has to be created for this controller (default is a lazy create on use)
  This can be turned on by a controller config and session data will be created immediately instead of lazy
  */
  bool isSessionRequired() const;

  /*!
  Command specified gzip of the output
  If something invalid or out of range is specified then 0 is assumed
  
  @return 0-9, 0=off, 1=minimum, 9=maximum
  */
  int getGZipLevel() const;

  /*!
  Gets the path of the command with command name

  @return absolute base path + command name
  */
  const AString& getPath() const;

  /*!
  Gets the absolute base path of the request URL
  Format of '/somepath/'

  @return absolute base path (without command name)
  */
  void emitBasePath(AOutputBuffer&) const;

  /*!
  Returns alias to use instead of this command
  If empty then this is a valid command else lookup the alias name
  */
  const AString& getAlias() const;
  
  /*!
  Name of the input processor
  Often the MIME type of the HTTP request
  */
  const AString& getInputProcessorClassName() const;
  
  /*!
  Name of the output generator
  */
  const AString& getOutputGeneratorClassName() const;

  /*!
  Input processor parameters
  */
  const AXmlElement& getInputParams() const;
  
  /*!
  Output generator parameters
  */
  const AXmlElement& getOutputParams() const;

  /*!
  Get module container
  */
  const AOSModules& getModules() const;

  /*!
  AOSAdminInterface
  */
  virtual void adminRegisterObject(AOSAdminRegistry& adminRegistry);
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Command alias
  AString m_Path;

  //a_Command alias
  AString m_Alias;
  
  //a_Input processor
  AString m_InputProcessor;
  AXmlElement m_InputParams;

  //a_Output generator
  AString m_OutputGenerator;
  AXmlElement m_OutputParams;

  //a_Associated modules and the parameters
  AOSModules m_Modules;
  
  //a_Attributes
  bool m_Enabled;              //a_Command state, if disabled, static XML will be attempted instead
  bool m_ForceAjax;            //a_Ajax forces a lean XML document, false by default
  bool m_SessionRequired;      //a_Session data is created immediately and not lazy when set, false by default
  bool m_CacheControlNoCache;  //a_If a command allows Cache-Control: no-cache (default is true)
  int m_GZipLevel;             //a_Forced gzip compression level of the output

  //a_The log
  ALog& m_Log;
};

#endif // INCLUDED__AOSController_HPP__
