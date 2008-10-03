/*
Written by Alex Chachanashvili

$Id$
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
    for (MAP_Libraries::iterator it = m_Libraries.begin(); it != m_Libraries.end(); ++it)
      ::FreeLibrary((HMODULE)(*it).second);
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

  void *procAddress = ::GetProcAddress(hmodule, strSymbolName.c_str());
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
