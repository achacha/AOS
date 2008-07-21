/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSInputExecutor_HPP__
#define INCLUDED__AOSInputExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSContext.hpp"

class AOSServices;
class AOSInputProcessorInterface;
class AOSAdminRegistry;

class AOS_BASE_API AOSInputExecutor : public AOSAdminInterface
{
public:
  AOSInputExecutor(AOSServices&);
  virtual ~AOSInputExecutor();

  //! Execute module specified in request header content-type field
  virtual void execute(AOSContext&);

  /*!
   * The executor owns the input processors and will delete them when done
   * NOTE: Default calls init(), if this method is overridden that needs to be accounted for
  **/
  virtual void registerInputProcessor(AOSInputProcessorInterface *);

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
  static const AString OVERRIDE_INPUT;
  
  AOSServices& m_Services;

  typedef std::map<AString, AOSInputProcessorInterface *> InputProcessorContainer;
  InputProcessorContainer m_InputProcessors;
};

#endif // INCLUDED__AOSInputExecutor_HPP__
