#ifndef INCLUDED_AOSModuleInfo_HPP__
#define INCLUDED_AOSModuleInfo_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"

class AOS_BASE_API AOSModuleInfo : public ADebugDumpable
{
public:
  /*!
  Create module info
  */
  AOSModuleInfo(const AString& className, const AXmlElement& paramsBase);

  /*!
  Get module class
  */
  const AString& getModuleClass() const;

  /*!
  Get module parameters
  */
  const AXmlElement& getParams() const;

  /*!
  Use module parameters
  */
  AXmlElement& useParams();

private:
  //! No default ctor
  AOSModuleInfo() {}
  
  //! Classname
  AString m_Class;
  
  //! Module parameters
  AXmlElement m_ModuleParams;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};



#endif
