/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSAdminInterface_HPP__
#define INCLUDED__AOSAdminInterface_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AEmittable.hpp"
#include "AMovingAverage.hpp"
#include "AXmlElement.hpp"

class AHTTPRequestHeader;
class AOSAdminRegistry;

/*!
Interface guarantees that a registered admin object can generate XML specific to its state and otionally process actions

\verbatim
-----CUT: Add in your inherited class-----
public:
  //REQUIRED
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;
  
  //OPTIONAL (if any actions are attached to properties)
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminRegisterObject(AOSAdminRegistry& registry);
  virtual void adminRegisterObject(AOSAdminRegistry& registry, const AString& parentClassName);

-----CUT: Add in your inherited class-----

-----CUT: Add in your inherited class implementation-----
const AString& MyClass::getClass() const
{
  static const AString CLASS("MyClass");
  return CLASS;   // MUST be unique with all AOSAdminInterface based classes
}
void MyClass::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{ 
  AOSAdminInterface::adminEmitXml(eBase, request);

  //TODO: Your code here
}
-----CUT: Add in your inherited class implementation-----
\endverbatim
*/
class AOS_BASE_API AOSAdminInterface : public ADebugDumpable
{
public:
  //! ctor
  AOSAdminInterface();
  
  //! virtual dtor
  virtual ~AOSAdminInterface();

  /*!
  Process object's actions

  Build the XML for this object if 'action' is not specified

  Derived methods must call this method also

  Example:
  \code
  void adminRegisterObject(AOSAdminRegistry& registry)
  {
    // register self
    registry.adminRegisterObject("MyClass", *this);

    // can register subclasses also
  }
  void MyClass::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
  {
    //NOTE: Call the parent class here, which would call it's parent until AOSAdminInterface is reached
    //  If a parent class does not have thsi method, it should be added to complete the chain
    AOSAdminInterface::adminEmitXml(eBase, request);

    // add properties for this object here
  }
  void MyClass::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
  {
    // process 'request' and apply any requested changes
  }
  \endcode
  */
  virtual void adminRegisterObject(AOSAdminRegistry& adminRegistry);
  virtual void adminRegisterObject(AOSAdminRegistry& adminRegistry, const AString& parentClassName);
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);

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
  Admin XML helper

  Encoding for propertyValue is assumed AXmlElement::None, use adminAddProperty(...) helper to use a different one
  if the propertyValue is not XML content safe and requires some CDATA wrapping/encoding

  @param eBase AXmlElement that is the base for this property
  @param propertyName name
  @param propertyValue value
  @param actionName name
  @param actionDesc description
  @return newly created property element
  */
  AXmlElement& adminAddPropertyWithAction(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue,
    const AString& actionName,
    const AString& actionDesc
  ) const;

  /*!
  Admin XML helper

  Encoding for propertyValue is assumed AXmlElement::None, use adminAddProperty(...) helper to use a different one
  if the propertyValue is not XML content safe and requires some CDATA wrapping/encoding

  @param eBase AXmlElement that is the base for this property
  @param propertyName name
  @param propertyValue value
  @param actionName name
  @param actionDesc description
  @param actionParam that will be used in the form for submit
  @return newly created property element
  */
  AXmlElement& adminAddPropertyWithAction(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue,
    const AString& actionName,
    const AString& actionDesc,
    const AString& actionParam
  ) const;

  /*!
  Admin XML helper

  Encoding for propertyValue is assumed AXmlElement::None, use adminAddProperty(...) helper to use a different one
  if the propertyValue is not XML content safe and requires some CDATA wrapping/encoding

  @param eBase AXmlElement that is the base for this property
  @param propertyName name
  @param propertyValue value
  @param actionName name
  @param actionDesc description
  @param actionParams list that will be used in the form for submit
  @return newly created property element
  */
  AXmlElement& adminAddPropertyWithAction(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue,
    const AString& actionName,
    const AString& actionDesc,
    const LIST_AString& actionParams
  ) const;
  
  /*!
  Add error

  @param eBase AXmlElement that is the base for this property
  @param adminError error
  @param adminErrorEncoding encoding to use on error
  @return newly created property element
  */
  AXmlElement& adminAddError(
    AXmlElement& eBase, 
    const AString& adminError,
    AXmlElement::Encoding adminErrorEncoding = AXmlElement::ENC_CDATADIRECT
  ) const;

  /*!
  Add message

  @param eBase AXmlElement that is the base for this property
  @param adminMessage message
  @param adminMessageEncoding encoding to use on message
  @return newly created property element
  */
  AXmlElement& adminAddMessage(
    AXmlElement& eBase, 
    const AString& adminMessage,
    AXmlElement::Encoding adminMessageEncoding = AXmlElement::ENC_CDATADIRECT
  ) const;

  /*!
  Finer grain helper: returns property element created
  Allows you to specify encoding for the propertyValue

  @param eBase AXmlElement that is the base for this property
  @param propertyName name
  @param propertyValue value
  @param propertyValueEncoding to use on value
  @return newly created property element
  */
  AXmlElement& adminAddProperty(
    AXmlElement& eBase, 
    const AString& propertyName, 
    const AEmittable& propertyValue, 
    AXmlElement::Encoding propertyValueEncoding = AXmlElement::ENC_NONE
  ) const;

  /*!
  Finer grain helper: returns action element attached to the property

  @param eProperty property AXmlElement
  @param actionName action name to attach
  @param actionDesc action description
  @return newly created property element
  */
  AXmlElement& adminAddActionToProperty(AXmlElement& eProperty, const AString& actionName, const AString& actionDesc) const;

  /*!
  Finer grain helper: returns action element attached to the property

  @param eProperty property AXmlElement
  @param actionName action name to attach
  @param actionDesc action description
  @param actionParam action parameter
  @return newly created property element
  */
  AXmlElement& adminAddActionToProperty(AXmlElement& eProperty, const AString& actionName, const AString& actionDesc, const AString& actionParam) const;

  /*!
  Finer grain helper: returns action element attached to the property

  @param eProperty property AXmlElement
  @param actionName action name to attach
  @param actionDesc action description
  @param actionParams list of action parameters
  @return newly created property element
  */
  AXmlElement& adminAddActionToProperty(AXmlElement& eProperty, const AString& actionName, const AString& actionDesc, const LIST_AString& actionParams) const;

protected:
  AMovingAverage m_ExecutionTimeAverage;
};

#endif // INCLUDED__AOSAdminInterface_HPP__
