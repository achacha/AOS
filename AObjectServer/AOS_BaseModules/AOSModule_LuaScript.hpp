/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSModule_LuaScript_HPP__
#define INCLUDED__AOSModule_LuaScript_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
Inline Lua script module
Loads and evaluates a Lua script, output is written to the model as CDATA
'outpath' is relative to the root element output document or absolute starting with /root/{your outpath}/script

Sample usage:

<module class="LuaScript">
  <filename>lua/myscript.lua</filename>    This will read from {datadir}/lua/myscript.lua
  <outpath>script</outpath>                This will output to /root/script/output
</module>

<module class="LuaScript">
  <script><![CDADA[
-- Your lua script here
print("Hello World!");
  ]]></script>
  <outpath>script</outpath>
</module>

<module class="LuaScript">
  <script><![CDADA[
-- More lua script here
print("This is a test.");
  ]]></script>
  <outpath>script</outpath>
</module>


Here results will be:
<root>
  <script>
    <output source="lua/myscript.lua"><![CDATA[something here output from the script file]]></output>
    <output source="Inline"><![CDATA[Hello World!]]></output>
    <output source="Inline"><![CDATA[This is a test]]></output>
  </script>
  ...
</root>

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
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_LuaScript_HPP__
