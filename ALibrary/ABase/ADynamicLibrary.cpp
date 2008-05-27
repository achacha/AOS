/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "ADynamicLibrary.hpp"
#include "AException.hpp"

#ifndef __WINDOWS__
  #pragma error("ADynamicLibrary not implemented yet")
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
    MAP_Libraries::iterator it = m_Libraries.begin();
    while (it != m_Libraries.end())
    {
      ::FreeLibrary((HMODULE)(*it).second);
      ++it;
    }
  }
  catch(...){}
}

bool ADynamicLibrary::load(const AString& strLibraryName)
{
  return (__load(strLibraryName) == NULL ? false : true);
}

void *ADynamicLibrary::__load(const AString& strLibraryName)
{
  MAP_Libraries::iterator it = m_Libraries.find(strLibraryName);
  if (it != m_Libraries.end())
  {
    ATHROW_EX(this, AException::InvalidParameter, AString("Library already loaded: ") + strLibraryName);
  }

  AString str(strLibraryName);
  str.append(".dll", 4);
  HMODULE hmodule = ::LoadLibrary(str.c_str());
  void *pLibrary = (void *)hmodule;

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

  m_Libraries.erase(it);
  HMODULE hmodule = (HMODULE)((*it).second);
  return (::FreeLibrary(hmodule) ? true : false);
}

void *ADynamicLibrary::getEntryPoint(const AString& strLibraryName, const AString& strSymbolName)
{
  MAP_Libraries::iterator it = m_Libraries.find(strLibraryName);
  HMODULE hmodule = 0;
  if (it == m_Libraries.end())
  {
    void *pLibrary = __load(strLibraryName);
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

  void *procAddress = ::GetProcAddress(hmodule, strSymbolName.c_str());
  return procAddress;
}

