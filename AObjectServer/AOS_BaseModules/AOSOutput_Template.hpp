/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSOutput_Template_HPP__
#define INCLUDED__AOSOutput_Template_HPP__

#include "apiAOS_BaseModules.hpp"

class ATemplate;

/*!
Example:

...
<module class="Template">
  <filename if="...">script/something.html</filename>
  <filename ifnot="...">script/something_not.html</filename>
  <filename if="...">script/something_more.html</filename>
</module>
...

*/
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
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
};

#endif //INCLUDED__AOSOutput_Template_HPP__
