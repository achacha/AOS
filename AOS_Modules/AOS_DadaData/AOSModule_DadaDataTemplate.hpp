#ifndef INCLUDED__AOSModule_DadaDataTemplate_HPP__
#define INCLUDED__AOSModule_DadaDataTemplate_HPP__

#include "apiAOS_DadaData.hpp"
#include "AOSModuleInterface.hpp"
#include "AString.hpp"

class ADadaDataHolder;
class AXmlElement;

class AOS_DADADATA_API AOSModule_DadaDataTemplate : public AOSModuleInterface
{
public:
  AOSModule_DadaDataTemplate(AOSServices&);
  
  virtual void init();
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);
  virtual void deinit();

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  typedef std::map<AString, VECTOR_AString> TEMPLATES;
  TEMPLATES m_Templates;

  typedef MAP_AString_AString VARIABLEMAP;

  void _generateLine(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& format, AXmlElement&);
  void _appendWordType(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& strType, AOutputBuffer&);
  void _appendVariable(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& strType, AOutputBuffer&);
  void _parseAttributes(const AString& data, AXmlAttributes&);
};

#endif // INCLUDED__AOSModule_DadaDataTemplate_HPP__

/*!
Template usage
---
{%TYPE} - looks up a type and replaces it
{%TYPE:ATTRIBUTE,ATTRIBUTE,...} - Generate a type with attribute
{%TYPE:$VARIABLE} - Get a type and assign it to a variable (see next line)
{$VARIABLE} - replace a generated variable (from above)


ATTRIBUTEs applied to generated words (in order of evaluation)
---
$VARIABLE - Any name starting with $ are stored in the variable container for usage later in the template
article - prepend with a or an
plural - attempt to make the word plural
lowercase - convert word to lowercase
uppercase - convert word to uppercase
proper - capitalize first letter

%verb:
  present - convert to -ing
  past - convert to -ed -d ending

*/