#ifndef INCLUDED__AOSDirectoryConfig_HPP__
#define INCLUDED__AOSDirectoryConfig_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"
#include "AXmlParsable.hpp"
#include "AOSAdminInterface.hpp"

class AFile;
class ALog;

class AOS_BASE_API AOSDirectoryConfig : public AXmlEmittable, public AOSAdminInterface, public AXmlParsable
{
public:
  struct MODULE_INFO
  {
    MODULE_INFO() : m_ModuleParams(ASW("params",6)) {}
    AString m_Name;
    AString m_Class;
    AXmlElement m_ModuleParams;
  };
  typedef std::list<MODULE_INFO> MODULES;

public:
  AOSDirectoryConfig(const AString& name, ALog&);
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
   Access to command data
  */
  inline bool isEnabled() const;
  inline bool isSession() const;
  inline bool isForceAjax() const;
  inline const AString& getCommandName() const;
  inline const AString& getInputProcessorName() const;
  inline const AString& getOutputGeneratorName() const;

  /*!
  Get module parameters as a query string (convenient string=string container)
  Returns false if no such module is found
  */
  const AXmlElement& getInputParams() const;
  const AXmlElement& getOutputParams() const;

  /*!
  Get module container
  */
  const MODULES& getModuleInfoContainer() const;

  /*!
  Command name
  */
  AString& useCommandName();

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  //a_String constants
  static const AString CLASS;

private:
  //a_Commands
  AString m_Command;
  
  //a_Input processor
  AString m_InputProcessor;
  AXmlElement m_InputParams;

  //a_Output generator
  AString m_OutputGenerator;
  AXmlElement m_OutputParams;

  //a_Associated modules and the parameters
  MODULES m_Modules;
  
  //a_Attributes
  bool m_Enabled;            //a_Command state, if disabled, static XML will be attempted instead
  bool m_Session;            //a_By default all commands are session based unless turned off with 0 or false
  bool m_ForceAjax;          //a_Ajax forces a lean XML document, false by default

  //a_The log
  ALog& m_Log;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

/*
XML for each command (example below)

<?xml version="1.0" encoding="UTF-8"?>
<command>
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

The parameters will be put into a <params> element.
E.g. for above first module:
<params>
  <name>foo</name>
  <param>0</param>
</params>
*/

#endif // INCLUDED__AOSDirectoryConfig_HPP__
