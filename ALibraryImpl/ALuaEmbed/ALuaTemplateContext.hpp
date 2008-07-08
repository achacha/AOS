/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ALuaTemplateContext_HPP__
#define INCLUDED__ALuaTemplateContext_HPP__

#include "apiALuaEmbed.hpp"
#include "ATemplateContext.hpp"
#include "ALuaEmbed.hpp"

/*!
Template context that contains an instance of Lua interpreter (one per thread)
*/
class ALuaEMBED_API ALuaTemplateContext : public ATemplateContext
{
public:
  /*!
  Construct the context

  @param objects available to the node handlers
  @param model XML data

  @param maskLibrariesToLoad which Lua libraries to load (See ALuaEmbed.hpp)
  */
  ALuaTemplateContext(
    ABasePtrContainer& objects, 
    AXmlDocument& model,
    AEventVisitor& visitor,
    u4 maskLibrariesToLoad = ALuaEmbed::LUALIB_ALL_SAFE
  );
  
  /*!
  */
  virtual ~ALuaTemplateContext();

  /*!
  Access Lua interpreter (lazy-eval, created first time this method is called)
  
  First time you use it will initialize lua and load libraries

  NOTE: Make sure all config of user defined libraries is done before calling this for the first time
  NOTE: Each instance of ALuaEmbed contains state which should not be shared by threads
  */
  ALuaEmbed& useLua();
  
  /*!
  Add external (user-defined) library to load when Lua is initialized
  */
  void addUserDefinedLibrary(ALuaEmbed::LUA_OPENLIBRARY_FPTR fptr);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! Lua interpreter
  ALuaEmbed *mp_Lua;

  //! Which lua libraries to load
  u4 m_LibrariesToLoad;

  /*!
  Queue up the library methods to call when Lua is initialized
  */
  typedef std::list<ALuaEmbed::LUA_OPENLIBRARY_FPTR> USER_DEFINED_FPTRS;
  USER_DEFINED_FPTRS m_UserDefinedLibFunctions;

};

#endif //INCLUDED__ALuaTemplateContext_HPP__
