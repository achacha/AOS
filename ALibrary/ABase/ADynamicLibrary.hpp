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
  
  //! dtor
  virtual ~ADynamicLibrary();

  /*!
  Load and Unload a dynamic library, return true is load successful
  Name is without the extension (e.g. MyLib will map to MyLib.dll on windows, libMyLib.so on unix, etc)
   i.e. On windows to load MyLib.dll you use load("MyLib") and .dll will be appended by this class
  */
  bool load(const AString& strLibraryName);
  bool unload(const AString& strLibraryName);

  /*!
  Will load library if needed and return address of the symbol, NULL if symbol or library not found
  */
  void *getEntryPoint(const AString& strLibraryName, const AString& strSymbolName);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private :
  typedef std::map<AString, void *> MAP_Libraries;
  MAP_Libraries m_Libraries;

  //a_Internal load method, returns pointer to a library handle
  void *__load(const AString& strLibraryName);

  //a_Disallow copying
  ADynamicLibrary(const ADynamicLibrary&) {}
  ADynamicLibrary& operator=(const ADynamicLibrary&) { return *this; }
};

#endif
