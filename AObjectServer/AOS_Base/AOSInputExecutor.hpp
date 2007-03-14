#ifndef INCLUDED__AOSInputExecutor_HPP__
#define INCLUDED__AOSInputExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;
class AOSContext;
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
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  AOSServices& m_Services;

  typedef std::map<AString, AOSInputProcessorInterface *> InputProcessorContainer;
  InputProcessorContainer m_InputProcessors;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream&, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSInputExecutor_HPP__
