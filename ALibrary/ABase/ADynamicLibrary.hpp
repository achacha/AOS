/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ADynamicLibrary_HPP__
#define INCLUDED__ADynamicLibrary_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"

class ABASE_API ADynamicLibrary : public ADebugDumpable
{
public :
  //! ctor
  ADynamicLibrary();
  
  /*!
  dtor
  Unloads all loaded libraries and release system resources
  */
  virtual ~ADynamicLibrary();

  /*!
  Load and Unload a dynamic library, return true is load successful
  Name is without the extension (e.g. MyLib will map to MyLib.dll on windows, libMyLib.so on unix, etc)
   i.e. On windows to load MyLib.dll you use load("MyLib") and .dll will be appended by this class
  
  @param libraryName to load
  @return true if loaded successfully
  */
  bool load(const AString& libraryName);

  /*!
  Load and Unload a dynamic library, return true is load successful
  Name is without the extension (e.g. MyLib will map to MyLib.dll on windows, libMyLib.so on unix, etc)
   i.e. On windows to load MyLib.dll you use load("MyLib") and .dll will be appended by this class

  @param libraryName to unload
  @return true if unloaded successfully
  */
  bool unload(const AString& libraryName);

  /*!
  Will load library if needed and return address of the symbol

  @param libraryName to look up, will try to load it if not already loaded
  @param symbolName entry point to look up in the loaded library
  @return pointer to the entry point or NULL if symbol or library not found
  */
  void *getEntryPoint(const AString& strLibraryName, const AString& strSymbolName);

  /*!
  Get a list of loaded modules
  
  @param target to add module names to
  @return number of modules added to list
  */
  size_t getModuleNames(LIST_AString& target) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private :
  typedef std::map<AString, void *> MAP_Libraries;
  MAP_Libraries m_Libraries;

  //a_Internal load method, returns pointer to a library handle
  void *_load(const AString& strLibraryName);

  //a_Disallow copying
  ADynamicLibrary(const ADynamicLibrary&) {}
  ADynamicLibrary& operator=(const ADynamicLibrary&) { return *this; }
};

#endif
