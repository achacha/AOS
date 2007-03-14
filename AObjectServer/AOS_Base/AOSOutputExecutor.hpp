#ifndef INCLUDED__AOSOutputExecutor_HPP__
#define INCLUDED__AOSOutputExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;
class AOSContext;
class AOSOutputGeneratorInterface;

class AOS_BASE_API AOSOutputExecutor : public AOSAdminInterface
{
public:
  AOSOutputExecutor(AOSServices&);
  virtual ~AOSOutputExecutor();

  /*!
  Execute module specified in request header content-type field
  */
  virtual void execute(AOSContext&);

  /*!
  The executor owns the Output processors and will delete them when done
  */
  virtual void registerOutputGenerator(AOSOutputGeneratorInterface *);

  /*!
  Admin interface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  AOSServices& m_Services;
  
  typedef std::map<AString, AOSOutputGeneratorInterface *> OutputGeneratorContainer;
  OutputGeneratorContainer m_OutputGenerators;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream&, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSOutputExecutor_HPP__
