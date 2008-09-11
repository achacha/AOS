/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModule_LuaScript_HPP__
#define INCLUDED__AOSModule_LuaScript_HPP__

#include "apiAOS_BaseModules.hpp"

/*!
LuaScript module
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

  /module/filename -load the script file relative to the data directory path base
  
  OUTPUT:
  /module/path -path of where the result of the script execution is placed in the output xml document
                    If not found, output from the script is discarded

  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& moduleParams);
  
  /*!
  AOSAdminInterface
  */
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_LuaScript_HPP__
