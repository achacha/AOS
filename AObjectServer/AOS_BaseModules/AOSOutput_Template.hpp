#ifndef INCLUDED__AOSOutput_Template_HPP__
#define INCLUDED__AOSOutput_Template_HPP__

#include "apiAOS_BaseModules.hpp"

class ATemplate;

class AOS_BASEMODULES_API AOSOutput_Template : public AOSOutputGeneratorInterface
{
public:
  AOSOutput_Template(AOSServices&);
  virtual ~AOSOutput_Template();

  /*!
  Generate output
  */
  virtual AOSContext::ReturnCode execute(AOSContext&);
  
  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  //typedef std::map<const AString, ATemplate *> TEMPLATES;
  //TEMPLATES *mp_Templates;

  //bool m_isCachingEnabled;

  //ASynchronization *mp_TemplatesGuard;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSOutput_Template_HPP__
