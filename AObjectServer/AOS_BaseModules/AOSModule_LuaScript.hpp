#ifndef INCLUDED__AOSModule_LuaScript_HPP__
#define INCLUDED__AOSModule_LuaScript_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Template module
Loads and evaluates a template and optionally writes output to an element

Sample usage:

...
<module class="Template">
  <template><![CDADA[
...
  ]]></template>
  <outpath>template/output</outpath>
</module>
...

outpath is relative to the root element output document or absolute starting with /root/

*/
class AOS_BASEMODULES_API AOSModule_LuaScript : public AOSModuleInterface
{
public:
  AOSModule_LuaScript(AOSServices&);

  /*!
  Execute a Lua script

  INPUT:
  /module/script  -script inside the command file
  
  or 

  /module/filename  -load the script file relative to the data directory path base
  
  OUTPUT:
  /module/outpath  -path of where the result of the script execution is placed in the output xml document
                    If not found, output from the script is discarded

  */
  virtual bool execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_LuaScript_HPP__
