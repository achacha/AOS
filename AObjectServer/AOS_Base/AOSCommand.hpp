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
  AOSCommand(const AString& name, ALog&);
  virtual ~AOSCommand();

  /*!
  Parse from XML element
  */
  virtual void fromXml(const AXmlElement&);

  /*!
  AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;

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
  Returns command path set when command is created
    not part of the XML rather the location of the actual command file
  */
  const AString& getCommandPath() const;

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
  virtual void registerAdminObject(AOSAdminRegistry& registry);
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;
  static const AString CLASS;

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
<command ajax='1'>
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
