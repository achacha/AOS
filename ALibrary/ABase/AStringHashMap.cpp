/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AStringHashMap.hpp"
#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AFile.hpp"
#include "AEmittable.hpp"
#include "ARope.hpp"
#include "ANameValuePair.hpp"
#include "ALock.hpp"

void AStringHashMap::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AStringHashMap @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_SynchObject=" << AString::fromPointer(mp_SynchObject) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Container={" << std::endl;
  for (size_t i=0; i<m_Container.size(); ++i)
  {
    MAP_AString_AString *pMap = m_Container.at(i);
    if (!pMap)
      ADebugDumpable::indent(os, indent+2) << "MAP_AString_AString[" << (u4)i << "]=NULL" << std::endl;
    else
    {
      ADebugDumpable::indent(os, indent+2) << "MAP_AString_AString[" << (u4)i << "][" << (u4)pMap->size() << "]={" << std::endl;
      MAP_AString_AString::const_iterator cit = pMap->begin();
      while (cit != pMap->end())
      {
        ADebugDumpable::indent(os, indent+3) << (*cit).first << "='" << (*cit).second << "'"<< std::endl;
        ++cit;
      }
      ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AStringHashMap::AStringHashMap(
  ASynchronization *pSynchObject,  // = NULL
  u1 hashSize                      // = 16
) :
  mp_SynchObject(pSynchObject)
{
  m_Container.resize(hashSize, NULL);
}

AStringHashMap::~AStringHashMap()
{
  try
  {
    for (size_t i=0; i<m_Container.size(); ++i)
      delete m_Container.at(i);

    delete mp_SynchObject;
  }
  catch(...) {}
}

void AStringHashMap::set(const AString& name, const AString& value)
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (!pMap)
  {
    pMap = new MAP_AString_AString();
    m_Container[hash] = pMap;
  }

  (*pMap)[name] = value;
}

void AStringHashMap::set(const AString& name, const ARope& value)
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (!pMap)
  {
    pMap = new MAP_AString_AString();
    m_Container[hash] = pMap;
  }

  (*pMap)[name] = AConstant::ASTRING_EMPTY;
  (*pMap)[name].setSize(value.getSize());
  value.emit((*pMap)[name]);
}

void AStringHashMap::set(const AString& name, const AEmittable& value)
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (!pMap)
  {
    pMap = new MAP_AString_AString();
    m_Container[hash] = pMap;
  }

  (*pMap)[name] = AConstant::ASTRING_EMPTY;
  ARope rope;
  value.emit(rope);
  (*pMap)[name].setSize(rope.getSize());
  value.emit((*pMap)[name]);
}

bool AStringHashMap::get(const AString& name, AOutputBuffer& target) const
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (pMap && pMap->find(name) != pMap->end())
  {
    (*pMap)[name].emit(target);
    return true;
  }
  else
    return false;
}

AString& AStringHashMap::use(const AString& name)
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (pMap && pMap->find(name) != pMap->end())
  {
    return (*pMap)[name];
  }
  else
    ATHROW_EX(this, AException::DoesNotExist, name);
}

const AString& AStringHashMap::get(const AString& name) const
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (pMap && pMap->find(name) != pMap->end())
  {
    return (*pMap)[name];
  }
  else
    ATHROW_EX(this, AException::DoesNotExist, name);
}

bool AStringHashMap::exists(const AString& name) const
{
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (pMap)
    return (pMap->find(name) == pMap->end() ? false : true);
  else
    return false;
}

bool AStringHashMap::remove(const AString& name)
{
  ALock lock(mp_SynchObject);
  size_t hash = name.getHash(m_Container.size());
  MAP_AString_AString *pMap = m_Container.at(hash);
  if (pMap)
  {
    pMap->erase(name);
    return true;
  }
  else
    return false;
}

void AStringHashMap::emit(AOutputBuffer& target) const
{
  ANameValuePair nvp(ANameValuePair::CGI_CASE);
  HASH_MAP::const_iterator citMap = m_Container.begin();
  bool emittedSome = false;
  while(citMap != m_Container.end())
  {
    MAP_AString_AString *pMap = *citMap;
    if (pMap)
    {
      MAP_AString_AString::const_iterator cit = pMap->begin();
      while (cit != pMap->end())
      {
        nvp.setNameValue((*cit).first, (*cit).second);
        nvp.emit(target);
        nvp.clear();
        ++cit;
        emittedSome = true;

        if (cit != pMap->end())
          target.append('&');
      }
    }
    ++citMap;

    if (emittedSome && citMap != m_Container.end() && *citMap && (*citMap)->size() > 0)
      target.append('&');
  }
}

AXmlElement& AStringHashMap::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  for (size_t i=0; i<m_Container.size(); ++i)
  {
    MAP_AString_AString *pMap = m_Container.at(i);
    if (pMap)
      for(MAP_AString_AString::const_iterator cit = pMap->begin(); cit != pMap->end();++cit)
        thisRoot.addElement((*cit).first, (*cit).second);
  }

  return thisRoot;
}

void AStringHashMap::toAFile(AFile& afile) const
{
  // FORMAT
  // buckets:data_size:data

  ALock lock(mp_SynchObject);

  //a_Save the hashmap buckets
  afile.write(AString::fromSize_t(m_Container.size()));
  afile.write(':');

  //a_Generate name/value pairs
  ARope rope;
  emit(rope);

  //a_Write size
  afile.write(AString::fromSize_t(rope.getSize()));
  afile.write(':');
  rope.emit(afile);
}

void AStringHashMap::fromAFile(AFile& afile)
{
  ALock lock(mp_SynchObject);
  
  //a_Size of the container
  AString str;
  AVERIFY(&afile, AConstant::npos != afile.readUntil(str, ':'));
  AASSERT(&afile, !str.isEmpty());
  m_Container.resize(str.toSize_t(), NULL);

  //a_Size of the data
  str.clear();
  AVERIFY(&afile, AConstant::npos != afile.readUntil(str, ':'));
  AASSERT(&afile, !str.isEmpty());
  size_t size = str.toSize_t();

  if (size > 0)
  {
    str.clear();
    afile.read(str, size);
    _parse(str);
  }
}

void AStringHashMap::_parse(const AString& data)
{
  size_t pos = 0;
  ANameValuePair pair(ANameValuePair::CGI_CASE);
  
  size_t len = data.getSize();
  while (pos < len)
  {
    pair.parse(data, pos);
    if (!pair.getName().isEmpty())
      set(pair.getName(), pair.getValue());

    pair.clear();
  }
}

size_t AStringHashMap::getSize() const
{
  size_t ret = 0;
  for (size_t i=0; i<m_Container.size(); ++i)
  {
    MAP_AString_AString *pMap = m_Container.at(i);
    if (pMap)
      ret += (u4)pMap->size();
  }
  return ret;
}

void AStringHashMap::clear()
{
  ALock lock(mp_SynchObject);
  for (size_t i=0; i<m_Container.size(); ++i)
  {
    MAP_AString_AString *pMap = m_Container.at(i);
    delete pMap;
    m_Container.at(i) = NULL;
  }
}
