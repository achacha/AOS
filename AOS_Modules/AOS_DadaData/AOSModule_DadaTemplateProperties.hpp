/*
Written by Alex Chachanashvili

$Id: AOSModule_DadaTemplateProperties.hpp 152 2008-12-01 14:05:26Z achacha $
*/
#ifndef INCLUDED__AOSModule_DadaTemplateProperties_HPP__
#define INCLUDED__AOSModule_DadaTemplateProperties_HPP__

#include "apiAOS_DadaData.hpp"
#include "AOSModuleInterface.hpp"
#include "AString.hpp"

class ADadaDataHolder;
class AXmlElement;

class AOS_DADADATA_API AOSModule_DadaTemplateProperties : public AOSModuleInterface
{
public:
  static const AString CLASS;

public:
  AOSModule_DadaTemplateProperties(AOSServices&);
  
  virtual AOSContext::ReturnCode execute(AOSContext&, const AXmlElement&);

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
};

#endif // INCLUDED__AOSModule_DadaTemplateProperties_HPP__
