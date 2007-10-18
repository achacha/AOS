#ifndef INCLUDED__AOSModule_Template_HPP__
#define INCLUDED__AOSModule_Template_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Lua script module

Sample usage:

...
<module class="LuaScript">
  <script><![CDADA[

  ]]></script>
  <outpath>/root/luascript/output</outpath>
</module>
...



*/
class AOS_BASEMODULES_API AOSModule_Template : public AOSModuleInterface
{
public:
  AOSModule_Template(AOSServices&);

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

#endif //INCLUDED__AOSModule_Template_HPP__
