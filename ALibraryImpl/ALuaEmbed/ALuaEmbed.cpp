
#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "ARope.hpp"
#include "ATemplate.hpp"

#ifdef __DEBUG_DUMP__
void ALuaEmbed::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_LuaState=" << AString::fromPointer(mp_LuaState) << std::endl;

  if (!mp_Objects.isNull())
  {
    ADebugDumpable::indent(os, indent+1) << "mp_Objects={" << std::endl;
    mp_Objects->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Objects=NULL" << std::endl;

  if (!mp_Output.isNull())
  {
    ADebugDumpable::indent(os, indent+1) << "mp_Output={" << std::endl;
    mp_Output->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Output=NULL" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ALuaEmbed::ALuaEmbed(
  u4 maskLibrariesToLoad
):
  mp_LuaState(NULL)
{
  mp_Objects.reset(new ABasePtrHolder());
  mp_Objects->insert(ATemplate::OBJECTNAME_MODEL, new AXmlDocument(ASW("root",4)), true);
  
  mp_Output.reset(new ARope());

  //a_Initialize Lua
  _init(maskLibrariesToLoad);
}

ALuaEmbed::ALuaEmbed(
  ABasePtrHolder& objects,
  AOutputBuffer& output,
  u4 maskLibrariesToLoad
):
  mp_LuaState(NULL)
{
  //a_We do not own these
  mp_Objects.reset(&objects, false);
  mp_Output.reset(&output, false);

  //a_Verify that model is in place since objects are provided externally
  useModel();

  //a_Initialize Lua
  _init(maskLibrariesToLoad);
}

ALuaEmbed::~ALuaEmbed()
{
  try
  {
    if (mp_LuaState)
      lua_close(mp_LuaState);
  }
  catch(...) {}
}

void ALuaEmbed::_init(u4 maskLibrariesToLoad)
{
  mp_LuaState = lua_open();
  AASSERT(NULL, mp_LuaState);
  mp_LuaState->mythis = this;  //a_Attach out output buffer
  lua_atpanic(mp_LuaState, ALuaEmbed::callbackPanic);

  //a_Explicit NONE selected, no libraries loaded
  if (LUALIB_NONE == maskLibrariesToLoad)
    return;

  //a_Load ALibrary based functions
  luaopen_alibrary(mp_LuaState);         // load ALibrary functions first (and override print() to use output)
  luaopen_base(mp_LuaState);             // load base Lua functions used by ALibrary
  luaopen_web(mp_LuaState);              // load web functions
  luaopen_objects(mp_LuaState);          // load objects functions
  luaopen_model(mp_LuaState);            // load model functions

  //a_Optional Lua libraries
  if (LUALIB_TABLE & maskLibrariesToLoad)   luaopen_table(mp_LuaState);            // opens the table library
  if (LUALIB_STRING & maskLibrariesToLoad)  luaopen_string(mp_LuaState);           // opens the string lib
  if (LUALIB_MATH & maskLibrariesToLoad)    luaopen_math(mp_LuaState);             // opens the math lib
  if (LUALIB_DEBUG & maskLibrariesToLoad)   luaopen_debug(mp_LuaState);            // opens the debug lib

//a_Following are not supported due to security concerns while being embedded, but can be uncommented if needed at your own risk  
//  if (LUALIB_OS & maskLibrariesToLoad)      luaopen_os(mp_LuaState);               // opens the OS lib
//  if (LUALIB_PACKAGE & maskLibrariesToLoad) luaopen_package(mp_LuaState);          // opens the package lib
//  if (LUALIB_IO & maskLibrariesToLoad)      luaopen_io(mp_LuaState);               // opens the I/O library
}

void ALuaEmbed::loadUserLibrary(LUA_OPENLIBRARY_FPTR fptr)
{
  fptr(mp_LuaState);
}

int ALuaEmbed::callbackPanic(lua_State *L)
{
  //a_We are here because Lua has paniced
  ATHROW_EX(NULL, AException::OperationFailed, ASWNL("Lua has had a panic attack."));
}

void ALuaEmbed::emit(AOutputBuffer& target) const
{
  AASSERT(this, !mp_Output.isNull());
  mp_Output->emit(target);
}

bool ALuaEmbed::execute(const AEmittable& code)
{
  AASSERT(NULL, mp_LuaState);
  try
  {
    ARope rope;
    code.emit(rope);
    const AString& buffer = rope.toAString();
    int error = luaL_loadbuffer(mp_LuaState, buffer.c_str(), buffer.getSize(), "buffer");
    if (error)
    {
      AString strError(lua_tostring(mp_LuaState, -1));
      lua_pop(mp_LuaState, 1);  // pop error message from the stack
      mp_Output->append(strError);
      return false;
    }

    //a_Execute LUA code
    switch(lua_pcall(mp_LuaState, 0, LUA_MULTRET, 0))
    {
      case LUA_ERRRUN:
      {
        AString strError("Runtime Error: ");
        strError.append(lua_tostring(mp_LuaState, -1));
        lua_pop(mp_LuaState, 1);  // pop error message from the stack
        mp_Output->append(strError);
        return false;
      } 
      break;
      
      case LUA_ERRMEM:
      {
        AString strError("Memory Allocation Error: ");
        strError.append(lua_tostring(mp_LuaState, -1));
        lua_pop(mp_LuaState, 1);  // pop error message from the stack
        mp_Output->append(strError);
        return false;
      } 
      break;

      case LUA_ERRERR:
      {
        AString strError("Error in error handler: ");
        strError.append(lua_tostring(mp_LuaState, -1));
        lua_pop(mp_LuaState, 1);  // pop error message from the stack
        mp_Output->append(strError);
        return false;
      }
      break;
    }
  }
  catch(AException& ex)
  {
    ex.emit(*mp_Output);
  }
  return true;
}

bool ALuaEmbed::execute(const AEmittable& code, ABasePtrHolder& objects, AOutputBuffer& output)
{
  //a_Override the defailts
  mp_Objects.reset(&objects, false);
  mp_Output.reset(&output, false);
  
  return execute(code);
}

AOutputBuffer& ALuaEmbed::useOutput()
{
  AASSERT(this, !mp_Output.isNull());
  return *mp_Output;
}

ABasePtrHolder& ALuaEmbed::useObjects()
{
  AASSERT(this, !mp_Objects.isNull());
  return *mp_Objects;
}

AXmlDocument& ALuaEmbed::useModel()
{
  AASSERT(this, !mp_Objects.isNull());
  AXmlDocument *pDoc = mp_Objects->useAsPtr<AXmlDocument>(ATemplate::OBJECTNAME_MODEL);
  if (pDoc)
    return *pDoc;
  else
    ATHROW_EX(this, AException::NotFound, ATemplate::OBJECTNAME_MODEL);
}

