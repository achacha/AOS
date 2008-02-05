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
  void _parseAttributes(const AString& data, AAttributes&);
};

#endif // INCLUDED__AOSModule_DadaDataTemplate_HPP__
