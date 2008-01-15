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

  static const AString PATH_DADADATA;        //a_Location of ADadaDataHolder in module object container
  static const AString PATH_OUTPUT;          //a_Location of the output

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  typedef std::map<AString, AString> VARIABLEMAP;
  VARIABLEMAP m_globals;

  void _generateLine(ADadaDataHolder *pddh, const AString& format, AXmlElement&);
  void _appendWordType(ADadaDataHolder *pddh, const AString& strType, AOutputBuffer&);
  void _appendVariable(ADadaDataHolder *pddh, const AString& strType, AOutputBuffer&);
  void _parseAttributes(const AString& data, AAttributes&);
};

#endif // INCLUDED__AOSModule_DadaDataTemplate_HPP__
