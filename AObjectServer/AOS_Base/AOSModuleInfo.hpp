/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED_AOSModuleInfo_HPP__
#define INCLUDED_AOSModuleInfo_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"

class AOSContext;

class AOS_BASE_API AOSModuleInfo : public ADebugDumpable
{
public:
  static AString COND_IF;
  static AString COND_IF_NOT;

public:
  /*!
  Create module info

  @param className of the module
  @param paramsBase element for the module parameters
  */
  AOSModuleInfo(const AString& className, const AXmlElement& paramsBase);

  /*!
  Get module class name
  
  @return module class name
  */
  const AString& getModuleClassName() const;

  /*!
  Get module parameters
  */
  const AXmlElement& getParams() const;

  /*!
  Use module parameters
  */
  AXmlElement& useParams();

  /*!
  Checks if the module should execute for this context

  @param context of the request
  */
  bool isExecute(AOSContext& context);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! No default ctor
  AOSModuleInfo() {}
  
  //! Classname
  AString m_ClassName;

  //! If or If-Not of the module
  AString m_If;
  AString m_IfNot;
  
  //! Module parameters
  AXmlElement m_ModuleParams;
};



#endif
