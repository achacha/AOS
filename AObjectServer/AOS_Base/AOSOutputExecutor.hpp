/*
Written by Alex Chachanashvili

$Id$
*/
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
  //! Class name
  static const AString CLASS;
  
  //! Output override parameter name
  static const AString OVERRIDE_OUTPUT;

public:
  AOSOutputExecutor(AOSServices&);
  virtual ~AOSOutputExecutor();

  /*!
  Checks if output generator with given class name exists

  @param className of the generator
  @return true if exists
  */
  bool exists(const AString& className) const;

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
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AOSServices& m_Services;
  
  typedef std::map<AString, AOSOutputGeneratorInterface *> OutputGeneratorContainer;
  OutputGeneratorContainer m_OutputGenerators;
};

#endif // INCLUDED__AOSOutputExecutor_HPP__
