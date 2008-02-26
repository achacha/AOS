#ifndef INCLUDED__AOSCommand_HPP__
#define INCLUDED__AOSCommand_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "AXmlParsable.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSModuleInfo.hpp"
#include "AOSModules.hpp"

class AFile;
class ALog;

class AOS_BASE_API AOSCommand : public AXmlEmittable, public AXmlParsable, public AOSAdminInterface
{
public:
  /*!
  Constant strings
  */
  static const AString S_COMMAND;
  static const AString S_INPUT;
  static const AString S_MODULE;
  static const AString S_OUTPUT;

  static const AString S_CLASS;
  static const AString S_ENABLED;
  static const AString S_AJAX;
  static const AString S_ALIAS;
  static const AString S_GZIP;

public:
  AOSCommand(const AString& name, ALog&);
  virtual ~AOSCommand();

  /*!
  Parse from XML element
  */
  virtual void fromXml(const AXmlElement&);

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
  Command specified gzip of the output
  If something invalid or out of range is specified then 0 is assumed
  
  @return 0-9, 0=off, 1=minimum, 9=maximum
  */
  int getGZipLevel() const;

  /*!
  Gets the path of the command with command name

  @return absolute base path + command name
  */
  const AString& getCommandPath() const;

  /*!
  Gets the absolute base path of the request URL
  Format of '/somepath/'

  @return absolute base path (without command name)
  */
  void emitCommandBasePath(AOutputBuffer&) const;

  /*!
  Returns alias to use instead of this command
  If empty then this is a valid command else lookup the alias name
  */
  const AString& getCommandAlias() const;
  
  /*!
  Name of the input processor
  Often the MIME type of the HTTP request
  */
  const AString& getInputProcessorName() const;
  
  /*!
  Name of the output generator
  */
  const AString& getOutputGeneratorName() const;

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
  virtual void adminRegisterObject(AOSAdminRegistry& registry);
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //a_Command alias
  AString m_CommandPath;

  //a_Command alias
  AString m_CommandAlias;
  
  //a_Input processor
  AString m_InputProcessor;
  AXmlElement m_InputParams;

  //a_Output generator
  AString m_OutputGenerator;
  AXmlElement m_OutputParams;

  //a_Associated modules and the parameters
  AOSModules m_Modules;
  
  //a_Attributes
  bool m_Enabled;            //a_Command state, if disabled, static XML will be attempted instead
  bool m_ForceAjax;          //a_Ajax forces a lean XML document, false by default
  int m_GZipLevel;           //a_Forced gzip compression level of the output

  //a_The log
  ALog& m_Log;
};

/*
XML for each command (example below)

<?xml version="1.0" encoding="UTF-8"?>
<command ajax='1' gzip='3' enabled='true'>
	<input class='application/x-www-form-urlencoded'/>
  <module class='PublishInput'>
    <name>foo</name>
    <param>0</param>
  </module>
  <module class='somemodule'>
    <name>bar</name>
    <state>2</state>
  </module>
	<output class='xslt'>
    <filename>aos.xsl</filename>
  </output>
</command>

<?xml version="1.0" encoding="UTF-8"?>
<command alias='/somepath/anothercommand'/>

*/

#endif // INCLUDED__AOSCommand_HPP__
