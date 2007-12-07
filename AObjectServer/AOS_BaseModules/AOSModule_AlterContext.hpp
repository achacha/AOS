#ifndef INCLUDED__AOSModule_AlterContext_HPP__
#define INCLUDED__AOSModule_AlterContext_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Alter RequestContext

add-parameter       = adds a name/value parameter to the request parameters
add-request-header  = adds to request header
add-response-header = adds to response header
<module class="AlterContext">
	<add-parameter>
		<name>param-added-0</name>
		<value>0</value>
	</add-parameter>
...

add-request-cookie
add-response-cookie
<module class="AlterContext">
  <add-request-cookie>
    <max-age>100</max-age>
    <domain>foo.com</domain>
    <path>/</path>
    <version>1</version>
    <expired/>
    <no-expire/>
    <secure/>
  </add-request-cookie>
...
*/
class AOS_BASEMODULES_API AOSModule_AlterContext : public AOSModuleInterface
{
public:
  AOSModule_AlterContext(AOSServices&);

  /*!
  Execute module
  */
  virtual bool execute(AOSContext& context, const AXmlElement& moduleParams);

  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;

private:
  //Set a cookie from element
  void _processSetCookie(ACookies&, AXmlElement::CONST_CONTAINER&);
};

#endif //INCLUDED__AOSModule_AlterContext_HPP__
