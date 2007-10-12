#ifndef INCLUDED__AOSAdminInterface_HPP__
#define INCLUDED__AOSAdminInterface_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AEmittable.hpp"
#include "AMovingAverage.hpp"

class AXmlElement;
class AHTTPRequestHeader;
class AOSAdminRegistry;

/*!
Interface guarantees that a registered admin object can generate XML specific to its state
and otionally process actions

-----CUT: Add in your inherited class-----
public:
  //REQUIRED
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;
  
  //OPTIONAL (if any actions are attached to properties)
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void registerAdminObject(AOSAdminRegistry& registry);
  virtual void registerAdminObject(AOSAdminRegistry& registry, const AString& parentClassName);

-----CUT: Add in your inherited class-----

-----CUT: Add in your inherited class implementation-----
const AString& MyClass::getClass() const
{
  static const AString CLASS("MyClass");
  return CLASS;   // MUST be unique with all AOSAdminInterface based classes
}
void MyClass::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{ 
  AOSAdminInterface::addAdminXml(eBase, request);

  //TODO: Your code here
}
-----CUT: Add in your inherited class implementation-----

*/
class AOS_BASE_API AOSAdminInterface : public ADebugDumpable
{
public:
  AOSAdminInterface() {}
  virtual ~AOSAdminInterface() {}

  /*!
  Process object's actions

  Build the XML for this object if 'action' is not specified

  Derived methods must call this method also

  Example:
  void registerAdminObject(AOSAdminRegistry& registry)
  {
    // register self
    registry.registerAdminObject("MyClass", *this);

    // can register subclasses also
  }
  void MyClass::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
  {
    //NOTE: Call the parent class here, which would call it's parent until AOSAdminInterface is reached
    //  If a parent class does not have thsi method, it should be added to complete the chain
    AOSAdminInterface::addAdminXml(eBase, request);

    // add properties for this object here
  }
  void MyClass::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
  {
    // process 'request' and apply any requested changes
  }
  */
  virtual void registerAdminObject(AOSAdminRegistry& registry);
  virtual void registerAdminObject(AOSAdminRegistry& registry, const AString& parentClassName);
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request) {}

  /*!
  Classname, MUST be unique to all AOSAdminInterface based classes
  */
  virtual const AString& getClass() const = 0;

  /*!
  Used in debug build to dump the debugDump of the object
  */
  static const AString DEBUGDUMP;      // "debugDump"
  
  /*!
  Statistics
  Used by ModuleExecutor to keep statistics on the module execution
  */
  void   addExecutionTimeSample(double milliseconds);
  u8     getExecutionCount() const;
  double getExecutionTimeAverage() const;

  /*!
  AString constants
  */
  static const AString ADMIN_ERROR;
  static const AString ADMIN_MESSAGE;
  static const AString NAME;
  static const AString PROPERTY;
    static const AString VALUE;
  static const AString ACTION;
    static const AString DESC;
    static const AString PARAM;

protected:
  /*!
  Admin XML helpers

  Encoding for propertyValue is assumed AXmlData::None, use addProperty(...) helper to use a different one
  if the propertyValue is not XML content safe and requires some CDATA wrapping/encoding

  Returns newly created <property> element
  */
  AXmlElement& addPropertyWithAction(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue,
    const AString& actionName,
    const AString& actionDesc
  ) const;

  AXmlElement& addPropertyWithAction(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue,
    const AString& actionName,
    const AString& actionDesc,
    const AString& actionParam
  ) const;

  AXmlElement& addPropertyWithAction(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue,
    const AString& actionName,
    const AString& actionDesc,
    const LIST_AString& actionParams
  ) const;
  
  /*!
  Add error
  */
  AXmlElement& addError(
    AXmlElement& eBase, 
    const AString& adminError,
    AXmlData::Encoding adminErrorEncoding = AXmlData::CDataDirect
  ) const;

  /*!
  Add message
  */
  AXmlElement& addMessage(
    AXmlElement& eBase, 
    const AString& adminMessage,
    AXmlData::Encoding adminMessageEncoding = AXmlData::CDataDirect
  ) const;

  /*!
  Finer grain helper: returns <property> element created
  Allows you to specify encoding for the propertyValue
  */
  AXmlElement& addProperty(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue, 
    AXmlData::Encoding propertyValueEncoding = AXmlData::None
  ) const;

  //a_Finer grain helper: returns <action> element attached to the property
  AXmlElement& addActionToProperty(AXmlElement& eProperty, const AString& actionName, const AString& actionDesc) const;
  AXmlElement& addActionToProperty(AXmlElement& eProperty, const AString& actionName, const AString& actionDesc, const AString& actionParam) const;
  AXmlElement& addActionToProperty(AXmlElement& eProperty, const AString& actionName, const AString& actionDesc, const LIST_AString& actionParams) const;

protected:
  AMovingAverage m_ExecutionTimeAverage;
};

#endif // INCLUDED__AOSAdminInterface_HPP__
