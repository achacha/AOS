/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ADynamicLibrary.hpp"
#include "AException.hpp"

#if defined(__LINUX__)
#include <dlfcn.h>
#endif


void ADynamicLibrary::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ")" << std::endl;
  MAP_Libraries::const_iterator cit = m_Libraries.begin();
  while (cit != m_Libraries.end())
  {
    ADebugDumpable::indent(os, indent+1) << (*cit).first << std::hex << (*cit).second << std::dec << std::endl;
  }
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ADynamicLibrary::ADynamicLibrary()
{
}

ADynamicLibrary::~ADynamicLibrary()
{
  try
  {
    for (MAP_Libraries::iterator it = m_Libraries.begin(); it != m_Libraries.end(); ++it)
#if defined(__WINDOWS__)
      ::FreeLibrary((HMODULE)(*it).second);
#elif defined(__LINUX__)
      dlclose((*it).second);
#endif
  }
  catch(...){}
}

bool ADynamicLibrary::load(const AString& strLibraryName)
{
  return (_load(strLibraryName) == NULL ? false : true);
}

void *ADynamicLibrary::_load(const AString& strLibraryName)
{
  MAP_Libraries::iterator it = m_Libraries.find(strLibraryName);
  if (it != m_Libraries.end())
  {
    ATHROW_EX(this, AException::InvalidParameter, AString("Library already loaded: ")+strLibraryName);
  }

  AString str(strLibraryName);
  void *pLibrary = NULL;
#if defined(__WINDOWS__)
  str.append(".dll", 4);
  HMODULE hmodule = ::LoadLibrary(str.c_str());
  pLibrary = (void *)hmodule;
#elif defined(__LINUX__)
  pLibrary = dlopen(str.c_str(), RTLD_NOW);
#endif

  if (!pLibrary)
  {
    ATHROW_EX(this, AException::NotFound, AString("Unable to load library: ")+strLibraryName);
  }
  m_Libraries[strLibraryName] = pLibrary;

  return pLibrary;
}

bool ADynamicLibrary::unload(const AString& strLibraryName)
{
  MAP_Libraries::iterator it = m_Libraries.find(strLibraryName);
  if (it == m_Libraries.end())
  {
    ATHROW_EX(this, AException::InvalidParameter, AString("Library not loaded: ") + strLibraryName);
  }

  HMODULE hmodule = (HMODULE)((*it).second);
  AASSERT(this, hmodule);
  m_Libraries.erase(it);
#if defined(__WINDOWS__)
  return (::FreeLibrary(hmodule) ? true : false);
#elif defined(__LINUX__)
  return 0 == dlclose(hmodule);
#endif
}

void *ADynamicLibrary::getEntryPoint(const AString& strLibraryName, const AString& strSymbolName)
{
  MAP_Libraries::iterator it = m_Libraries.find(strLibraryName);
  HMODULE hmodule = 0;
  if (it == m_Libraries.end())
  {
    void *pLibrary = _load(strLibraryName);
    if (!pLibrary)
    {
      ATHROW_EX(this, AException::InvalidParameter, AString("Library not loaded: ") + strLibraryName);
    }

    hmodule = (HMODULE)pLibrary;
  }
  else
  {
    hmodule = (HMODULE)((*it).second);
  }

  if (!hmodule)
    return NULL;

  void *procAddress = NULL;
#if defined(__WINDOWS__)
  procAddress = ::GetProcAddress(hmodule, strSymbolName.c_str());
#elif defined(__LINUX__)
  procAddress = dlsym(hmodule, strSymbolName.c_str());
#endif
  return procAddress;
}

size_t ADynamicLibrary::getModuleNames(LIST_AString& target) const
{
  for (MAP_Libraries::const_iterator cit = m_Libraries.begin(); cit != m_Libraries.end(); ++cit)
  {
    target.push_back((*cit).first);
  }
  return m_Libraries.size();
}
